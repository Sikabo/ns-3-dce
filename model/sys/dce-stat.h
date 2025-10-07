#ifndef DCE_STAT_H
#define DCE_STAT_H

#ifdef __cplusplus
extern "C" {
#endif

int dce_stat (const char *path, struct stat *buf);
int dce_fstat (int fd, struct stat *buf);
int dce_lstat (const char *pathname, struct stat *buf);

int dce_stat64 (const char *path, struct stat64 *buf);
int dce_fstat64 (int fd, struct stat64 *buf);
int dce_lstat64 (const char *pathname, struct stat64 *buf);
int dce_fstatat (int fd, const char *filename,
                    struct stat *buf, int flag);
int dce_fstatat64(int dirfd, const char * path,
                    struct stat64 * stat_buf, int flags);
int dce_fstat (int fd, struct stat *buf);
int dce_fstat64 (int fd, struct stat64 *buf);

int dce_mkdir (const char *pathname, mode_t mode);

#ifdef __cplusplus
}
#endif

#endif /* DCE_STAT_H */
