/*

 SD - a slightly more friendly wrapper for sdfatlib

 This library aims to expose a subset of SD card functionality
 in the form of a higher level "wrapper" object.

 License: GNU General Public License V3
          (Because sdfatlib is licensed with this.)

 (C) Copyright 2010 SparkFun Electronics

 */

#include <mySD.h>

/* for debugging file open/close leaks
   uint8_t nfilecount=0;
*/

MyFile::MyFile(SdFile f, const char *n) {
  // oh man you are kidding me, new() doesnt exist? Ok we do it by hand!
  _file = (SdFile *)malloc(sizeof(SdFile)); 
  if (_file) {
    memcpy(_file, &f, sizeof(SdFile));
    
    strncpy(_name, n, 12);
    _name[12] = 0;
    
    /* for debugging file open/close leaks
       nfilecount++;
       Serial.print("Created \"");
       Serial.print(n);
       Serial.print("\": ");
       Serial.println(nfilecount, DEC);
    */
  }
}

MyFile::MyFile(void) {
  _file = 0;
  _name[0] = 0;
  //Serial.print("Created empty file object");
}

MyFile::~MyFile(void) {
  //  Serial.print("Deleted file object");
}

// returns a pointer to the file name
char *MyFile::name(void) {
  return _name;
}

// a directory is a special type of file
boolean MyFile::isDirectory(void) {
  return (_file && _file->isDir());
}


size_t MyFile::write(uint8_t val) {
  return write(&val, 1);
}

size_t MyFile::write(const uint8_t *buf, size_t size) {
  size_t t;
  if (!_file) {
    setWriteError();
    return 0;
  }
  _file->clearWriteError();
  t = _file->write(buf, size);
  if (_file->getWriteError()) {
    setWriteError();
    return 0;
  }
  return t;
}

int MyFile::peek() {
  if (! _file) 
    return 0;

  int c = _file->read();
  if (c != -1) _file->seekCur(-1);
  return c;
}

int MyFile::read() {
  if (_file) 
    return _file->read();
  return -1;
}

// buffered read for more efficient, high speed reading
int MyFile::read(void *buf, uint16_t nbyte) {
  if (_file) 
    return _file->read(buf, nbyte);
  return 0;
}

int MyFile::available() {
  if (! _file) return 0;

  uint32_t n = size() - position();

  return n > 0X7FFF ? 0X7FFF : n;
}

void MyFile::flush() {
  if (_file)
    _file->sync();
}

boolean MyFile::seek(uint32_t pos) {
  if (! _file) return false;

  return _file->seekSet(pos);
}

uint32_t MyFile::position() {
  if (! _file) return -1;
  return _file->curPosition();
}

uint32_t MyFile::size() {
  if (! _file) return 0;
  return _file->fileSize();
}

void MyFile::close() {
  if (_file) {
    _file->close();
    free(_file); 
    _file = 0;

    /* for debugging file open/close leaks
    nfilecount--;
    Serial.print("Deleted ");
    Serial.println(nfilecount, DEC);
    */
  }
}

MyFile::operator bool() {
  if (_file) 
    return  _file->isOpen();
  return false;
}

