

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdatomic.h>

atomic_bool running = true;

// Receiver thread function
void *receiver_thread(void *arg) {
    int recv_sockfd = *(int *)arg;
    char buffer[1024];
    struct sockaddr_in sender_addr;
    socklen_t addr_len = sizeof(sender_addr);

    printf("Receiver thread started, listening for packets...\n");

    while (running) {
        ssize_t received = recvfrom(recv_sockfd, buffer, sizeof(buffer) - 1, 0,
                                   (struct sockaddr *)&sender_addr, &addr_len);

        if (received > 0) {
            buffer[received] = '\0';
            char sender_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &sender_addr.sin_addr, sender_ip, INET_ADDRSTRLEN);
            printf("RECEIVED from %s:%d: %s\n", sender_ip, ntohs(sender_addr.sin_port), buffer);
        } else if (received < 0 && running) {
            perror("recvfrom failed");
        }
    }

    printf("Receiver thread stopped\n");
    return NULL;
}

int main (int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <IP> <port> <num_packets>\n", argv[0]);
        return 1;
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);
    int num_packets = atoi(argv[3]);

    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Error: Invalid port number (must be 1-65535)\n");
        return 1;
    }

    if (num_packets <= 0) {
        fprintf(stderr, "Error: Number of packets must be positive\n");
        return 1;
    }

    // Create UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        return 1;
    }

    // Bind socket to receive on the same port
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        return 1;
    }

    printf("Socket bound to port %d for receiving\n", port);

    // Start receiver thread
    pthread_t recv_thread;
    if (pthread_create(&recv_thread, NULL, receiver_thread, &sockfd) != 0) {
        perror("Failed to create receiver thread");
        close(sockfd);
        return 1;
    }

    // Set up destination address
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &dest_addr.sin_addr) <= 0) {
        fprintf(stderr, "Error: Invalid IP address\n");
        close(sockfd);
        return 1;
    }

    printf("Sleeping for 1 second before sending packets...\n");
    sleep(1);

    // Send UDP packets
    char buffer[1024];
    for (int i = 0; i < num_packets; i++) {
        snprintf(buffer, sizeof(buffer), "UDP packet %d of %d", i + 1, num_packets);

        ssize_t sent = sendto(sockfd, buffer, strlen(buffer), 0,
                             (struct sockaddr *)&dest_addr, sizeof(dest_addr));

        if (sent < 0) {
            perror("sendto failed");
            close(sockfd);
            return 1;
        }

        printf("Sent packet %d: %s\n", i + 1, buffer);
        usleep(100000); // 100ms delay between packets
    }

    printf("Successfully sent %d UDP packets to %s:%d\n", num_packets, ip, port);

    // Wait a bit for any remaining packets to arrive
    printf("Waiting for incoming packets (5 seconds)...\n");
    sleep(5);

    // Stop receiver thread
    running = false;

    // Send a dummy packet to ourselves to unblock recvfrom
    sendto(sockfd, "STOP", 4, 0, (struct sockaddr *)&local_addr, sizeof(local_addr));

    pthread_join(recv_thread, NULL);

    close(sockfd);
    return 0;
}
