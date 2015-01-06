#ifndef CAFFE_UTIL_IO_H_
#define CAFFE_UTIL_IO_H_

#ifndef OSX
#include <opencv2/core/core.hpp>
#endif

#include <string>

#include "google/protobuf/message.h"

#ifndef _MSC_VER
  #include <unistd.h>
  #include "hdf5.h"
  #include "hdf5_hl.h"
#else
  #include <io.h>
  #include <fcntl.h>
  #include <sys\stat.h>
  #include <direct.h>
  #include <mutex>
#endif

#include "caffe/blob.hpp"
#include "caffe/proto/caffe.pb.h"

#define HDF5_NUM_DIMS 4

namespace caffe {

using ::google::protobuf::Message;

#ifdef _MSC_VER
void MakeTempDir(string* temp_dirname);
#endif

inline void MakeTempFilename(string* temp_filename) {
  
#ifdef _MSC_VER
  temp_filename->clear();
  std::string dirname;
  MakeTempDir(&dirname);
  *temp_filename = dirname + "\\caffe_test.XXXXXX";
  char* temp_filename_cstr = new char[temp_filename->size() + 1];
  strcpy(temp_filename_cstr, temp_filename->c_str());
  char *tmp = _mktemp(temp_filename_cstr);
  int fd = _open(tmp, _O_CREAT, _S_IWRITE);
  CHECK_GE(fd, 0) << "Failed to open a temporary file at: " << *temp_filename;
  _close(fd);
  *temp_filename = temp_filename_cstr;
  delete[] temp_filename_cstr;
#else
  temp_filename->clear();
  *temp_filename = "/tmp/caffe_test.XXXXXX";
  char* temp_filename_cstr = new char[temp_filename->size() + 1];
  // NOLINT_NEXT_LINE(runtime/printf)
  strcpy(temp_filename_cstr, temp_filename->c_str());
  int fd = mkstemp(temp_filename_cstr);
  CHECK_GE(fd, 0) << "Failed to open a temporary file at: " << *temp_filename;
  close(fd);
  *temp_filename = temp_filename_cstr;
  delete[] temp_filename_cstr;
#endif
}

inline void MakeTempDir(string* temp_dirname) {
#ifndef _MSC_VER
  temp_dirname->clear();
  *temp_dirname = "/tmp/caffe_test.XXXXXX";
  char* temp_dirname_cstr = new char[temp_dirname->size() + 1];
  // NOLINT_NEXT_LINE(runtime/printf)
  strcpy(temp_dirname_cstr, temp_dirname->c_str());
  char* mkdtemp_result = mkdtemp(temp_dirname_cstr);
  CHECK(mkdtemp_result != NULL)
	<< "Failed to create a temporary directory at: " << *temp_dirname;
  *temp_dirname = temp_dirname_cstr;
  delete[] temp_dirname_cstr;
#else
  static std::mutex mut;
  static int dir_count = 0;
  mut.lock();
  char buf[255];
  sprintf(buf, "./temp/%d", ++dir_count);
  _mkdir(buf);
  fflush(stdout);
  *temp_dirname = buf;
  mut.unlock();
#endif
}

bool ReadProtoFromTextFile(const char* filename, Message* proto);

inline bool ReadProtoFromTextFile(const string& filename, Message* proto) {
  return ReadProtoFromTextFile(filename.c_str(), proto);
}

inline void ReadProtoFromTextFileOrDie(const char* filename, Message* proto) {
  CHECK(ReadProtoFromTextFile(filename, proto));
}

inline void ReadProtoFromTextFileOrDie(const string& filename, Message* proto) {
  ReadProtoFromTextFileOrDie(filename.c_str(), proto);
}

void WriteProtoToTextFile(const Message& proto, const char* filename);
inline void WriteProtoToTextFile(const Message& proto, const string& filename) {
  WriteProtoToTextFile(proto, filename.c_str());
}

bool ReadProtoFromBinaryFile(const char* filename, Message* proto);

inline bool ReadProtoFromBinaryFile(const string& filename, Message* proto) {
  return ReadProtoFromBinaryFile(filename.c_str(), proto);
}

inline void ReadProtoFromBinaryFileOrDie(const char* filename, Message* proto) {
  CHECK(ReadProtoFromBinaryFile(filename, proto));
}

inline void ReadProtoFromBinaryFileOrDie(const string& filename,
                                         Message* proto) {
  ReadProtoFromBinaryFileOrDie(filename.c_str(), proto);
}


void WriteProtoToBinaryFile(const Message& proto, const char* filename);
inline void WriteProtoToBinaryFile(
    const Message& proto, const string& filename) {
  WriteProtoToBinaryFile(proto, filename.c_str());
}

bool ReadFileToDatum(const string& filename, const int label, Datum* datum);

inline bool ReadFileToDatum(const string& filename, Datum* datum) {
  return ReadFileToDatum(filename, -1, datum);
}

bool ReadImageToDatum(const string& filename, const int label,
    const int height, const int width, const bool is_color, Datum* datum);

inline bool ReadImageToDatum(const string& filename, const int label,
    const int height, const int width, Datum* datum) {
  return ReadImageToDatum(filename, label, height, width, true, datum);
}

inline bool ReadImageToDatum(const string& filename, const int label,
    const bool is_color, Datum* datum) {
  return ReadImageToDatum(filename, label, 0, 0, is_color, datum);
}

inline bool ReadImageToDatum(const string& filename, const int label,
    Datum* datum) {
  return ReadImageToDatum(filename, label, 0, 0, true, datum);
}

bool DecodeDatum(const int height, const int width, const bool is_color,
  Datum* datum);

inline bool DecodeDatum(const int height, const int width, Datum* datum) {
  return DecodeDatum(height, width, true, datum);
}

inline bool DecodeDatum(const bool is_color, Datum* datum) {
  return DecodeDatum(0, 0, is_color, datum);
}

inline bool DecodeDatum(Datum* datum) {
  return DecodeDatum(0, 0, true, datum);
}

#ifndef OSX
cv::Mat ReadImageToCVMat(const string& filename,
    const int height, const int width, const bool is_color);

inline cv::Mat ReadImageToCVMat(const string& filename,
    const int height, const int width) {
  return ReadImageToCVMat(filename, height, width, true);
}

inline cv::Mat ReadImageToCVMat(const string& filename,
    const bool is_color) {
  return ReadImageToCVMat(filename, 0, 0, is_color);
}

inline cv::Mat ReadImageToCVMat(const string& filename) {
  return ReadImageToCVMat(filename, 0, 0, true);
}

cv::Mat DecodeDatumToCVMat(const Datum& datum,
    const int height, const int width, const bool is_color);

inline cv::Mat DecodeDatumToCVMat(const Datum& datum,
    const int height, const int width) {
  return DecodeDatumToCVMat(datum, height, width, true);
}

inline cv::Mat DecodeDatumToCVMat(const Datum& datum,
    const bool is_color) {
  return DecodeDatumToCVMat(datum, 0, 0, is_color);
}

inline cv::Mat DecodeDatumToCVMat(const Datum& datum) {
  return DecodeDatumToCVMat(datum, 0, 0, true);
}

void CVMatToDatum(const cv::Mat& cv_img, Datum* datum);
#endif

#ifndef _MSC_VER
template <typename Dtype>
void hdf5_load_nd_dataset_helper(
  hid_t file_id, const char* dataset_name_, int min_dim, int max_dim,
  Blob<Dtype>* blob);

template <typename Dtype>
void hdf5_load_nd_dataset(
  hid_t file_id, const char* dataset_name_, int min_dim, int max_dim,
  Blob<Dtype>* blob);

template <typename Dtype>
void hdf5_save_nd_dataset(
  const hid_t file_id, const string dataset_name, const Blob<Dtype>& blob);
#endif
}  // namespace caffe

#endif   // CAFFE_UTIL_IO_H_
