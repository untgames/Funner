#include "shared.h"
#include <common/file.h>
using namespace common;


BadaFileSystem::file_t BadaFileSystem::FileOpen(const char* name,filemode_t mode_flags,size_t buffer_size) {
    throw xtl::make_not_implemented_exception("common::BadaFileSystem::FileOpen");
}


void BadaFileSystem::FileClose(file_t file) {
    throw xtl::make_not_implemented_exception("common::BadaFileSystem::FileClose");
}


size_t BadaFileSystem::FileRead(file_t file,void* buf,size_t size) {
    throw xtl::make_not_implemented_exception("common::BadaFileSystem::FileRead");
}


size_t BadaFileSystem::FileWrite(file_t file,const void* buf,size_t size) {
    throw xtl::make_not_implemented_exception("common::BadaFileSystem::FileWrite");
}


void BadaFileSystem::FileRewind(file_t file) {
    throw xtl::make_not_implemented_exception("common::BadaFileSystem::FileRewind");
}


filepos_t BadaFileSystem::FileSeek(file_t file,filepos_t pos) {
    throw xtl::make_not_implemented_exception("common::BadaFileSystem::FileSeek");
}


filepos_t BadaFileSystem::FileTell(file_t file) {
    throw xtl::make_not_implemented_exception("common::BadaFileSystem::FileTell");
}


filesize_t BadaFileSystem::FileSize(file_t file) {
    throw xtl::make_not_implemented_exception("common::BadaFileSystem::FileSize");
}


void BadaFileSystem::FileResize(file_t file,filesize_t new_size) {
    throw xtl::make_not_implemented_exception("common::BadaFileSystem::FileResize");
}


bool BadaFileSystem::FileEof(file_t file) {
    throw xtl::make_not_implemented_exception("common::BadaFileSystem::FileEof");
}


void BadaFileSystem::FileFlush(file_t file) {
    throw xtl::make_not_implemented_exception("common::BadaFileSystem::FileFlush");
}


void BadaFileSystem::Remove(const char* file_name) {
    throw xtl::make_not_implemented_exception("common::BadaFileSystem::Remove");
}


void BadaFileSystem::Rename(const char* file_name,const char* new_name) {
    throw xtl::make_not_implemented_exception("common::BadaFileSystem::Rename");
}


void BadaFileSystem::Mkdir(const char* dir_name) {
    throw xtl::make_not_implemented_exception("common::BadaFileSystem::Mkdir");
}


bool BadaFileSystem::IsFileExist(const char* file_name) {
    throw xtl::make_not_implemented_exception("common::BadaFileSystem::IsFileExist");
}


bool BadaFileSystem::GetFileInfo(const char* file_name,FileInfo& info) {
    throw xtl::make_not_implemented_exception("common::BadaFileSystem::FileSize");
}


void BadaFileSystem::Search(const char* mask,const FileSearchHandler& insert_handler) {
    throw xtl::make_not_implemented_exception("common::BadaFileSystem::Search");
}
