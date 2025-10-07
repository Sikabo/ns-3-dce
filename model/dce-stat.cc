#include <sys/stat.h>
#include <fcntl.h>
#include "sys/dce-stat.h"
#include "utils.h"
#include "process.h"
#include "ns3/log.h"
#include "ns3/assert.h"
#include <errno.h>
#include "file-usage.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("DceStat");

int dce_stat (const char *path, struct stat *buf)
{
  Thread *current = Current ();
  NS_LOG_FUNCTION (current << UtilsGetNodeId () << path << buf);
  NS_ASSERT (current != 0);
  if (std::string (path) == "")
    {
      current->err = ENOENT;
      return -1;
    }
  int retval = ::stat (UtilsGetRealFilePath (path).c_str (), buf);
  if (retval == -1)
    {
      current->err = errno;
      return -1;
    }
  return retval;
}
int dce_stat64 (const char *path, struct stat64 *buf)
{
  Thread *current = Current ();
  NS_LOG_FUNCTION (current << UtilsGetNodeId () << path << buf);
  NS_ASSERT (current != 0);
  if (std::string (path) == "")
    {
      current->err = ENOENT;
      return -1;
    }
  int retval = ::stat64 (UtilsGetRealFilePath (path).c_str (), buf);
  if (retval == -1)
    {
      current->err = errno;
      return -1;
    }
  return retval;
}
int dce_fstat (int fd, struct stat *buf)
{
  Thread *current = Current ();
  NS_LOG_FUNCTION (current << UtilsGetNodeId () << fd);
  NS_ASSERT (current != 0);

  OPENED_FD_METHOD (int, Fxstat (0, buf))
}
int dce_fstat64 (int fd, struct stat64 *buf)
{
  Thread *current = Current ();
  NS_LOG_FUNCTION (current << UtilsGetNodeId () << fd);
  NS_ASSERT (current != 0);

  OPENED_FD_METHOD (int, Fxstat64 (0, buf))
}
int dce_lstat (const char *pathname, struct stat *buf)
{
  Thread *current = Current ();
  NS_LOG_FUNCTION (current << UtilsGetNodeId () << pathname << buf);
  NS_ASSERT (current != 0);
  if (std::string (pathname) == "")
    {
      current->err = ENOENT;
      return -1;
    }
  int retval = ::lstat (UtilsGetRealFilePath (pathname).c_str (), buf);
  if (retval == -1)
    {
      current->err = errno;
      return -1;
    }
  return retval;
}
int dce_lstat64 (const char *pathname, struct stat64 *buf)
{
  Thread *current = Current ();
  NS_LOG_FUNCTION (current << UtilsGetNodeId () << pathname << buf);
  NS_ASSERT (current != 0);
  if (std::string (pathname) == "")
    {
      current->err = ENOENT;
      return -1;
    }
  int retval = ::lstat64 (UtilsGetRealFilePath (pathname).c_str (), buf);
  if (retval == -1)
    {
      current->err = errno;
      return -1;
    }
  return retval;
}
