// FIXME Danny
// This should be read from files, not hardcoded
unsigned char default_certificate[] = {
  0x30, 0x82, 0x01, 0x82, 0x30, 0x81, 0xec, 0x02, 0x09, 0x00, 0x88, 0xf2,
  0x5f, 0x46, 0x12, 0x2e, 0x3d, 0x3a, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86,
  0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x05, 0x05, 0x00, 0x30, 0x1c, 0x31,
  0x1a, 0x30, 0x18, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13, 0x11, 0x77, 0x77,
  0x77, 0x2e, 0x65, 0x73, 0x70, 0x72, 0x65, 0x73, 0x73, 0x69, 0x66, 0x2e,
  0x63, 0x6f, 0x6d, 0x30, 0x1e, 0x17, 0x0d, 0x31, 0x34, 0x30, 0x36, 0x32,
  0x34, 0x31, 0x30, 0x32, 0x32, 0x33, 0x33, 0x5a, 0x17, 0x0d, 0x32, 0x38,
  0x30, 0x33, 0x30, 0x32, 0x31, 0x30, 0x32, 0x32, 0x33, 0x33, 0x5a, 0x30,
  0x34, 0x31, 0x12, 0x30, 0x10, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13, 0x09,
  0x65, 0x73, 0x70, 0x72, 0x65, 0x73, 0x73, 0x69, 0x66, 0x31, 0x1e, 0x30,
  0x1c, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x15, 0x65, 0x73, 0x70, 0x72,
  0x65, 0x73, 0x73, 0x69, 0x66, 0x20, 0x49, 0x6f, 0x54, 0x20, 0x70, 0x72,
  0x6f, 0x6a, 0x65, 0x63, 0x74, 0x30, 0x5c, 0x30, 0x0d, 0x06, 0x09, 0x2a,
  0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x4b,
  0x00, 0x30, 0x48, 0x02, 0x41, 0x00, 0xb9, 0x83, 0x30, 0xca, 0xfb, 0xec,
  0x11, 0x9e, 0x94, 0xb7, 0x89, 0xf2, 0x84, 0x2c, 0xda, 0xe1, 0x9a, 0x53,
  0x3a, 0x1b, 0x6e, 0xc9, 0x85, 0x81, 0xf9, 0xa3, 0x41, 0xdb, 0xe2, 0x82,
  0x3b, 0xfa, 0x80, 0x22, 0x3b, 0x81, 0x6d, 0x25, 0x73, 0x7e, 0xf6, 0x49,
  0xcc, 0x69, 0x3c, 0x6c, 0xd8, 0x05, 0xfb, 0x92, 0x02, 0xcf, 0x19, 0x2a,
  0x10, 0x7d, 0x69, 0x7a, 0xd8, 0x9d, 0xd3, 0xcf, 0x6c, 0xef, 0x02, 0x03,
  0x01, 0x00, 0x01, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7,
  0x0d, 0x01, 0x01, 0x05, 0x05, 0x00, 0x03, 0x81, 0x81, 0x00, 0x2d, 0x63,
  0x58, 0x21, 0xe3, 0x8b, 0x37, 0x0d, 0x28, 0x68, 0x11, 0x0e, 0x4d, 0xdd,
  0xf3, 0xea, 0xdb, 0xec, 0xd7, 0x09, 0x47, 0x2c, 0xa1, 0xd8, 0xd1, 0x71,
  0x83, 0x11, 0xb4, 0x17, 0xbc, 0x83, 0xea, 0x5a, 0xd6, 0x73, 0x02, 0x25,
  0x87, 0x01, 0x76, 0xfc, 0x59, 0x1a, 0xcf, 0xd9, 0x49, 0xc9, 0xf9, 0x1f,
  0x5c, 0x3b, 0x24, 0x6a, 0x5c, 0xa5, 0xca, 0xe6, 0x5d, 0x34, 0x5b, 0x5f,
  0xcf, 0x56, 0x9c, 0x71, 0xd2, 0x6b, 0xdd, 0x1f, 0x15, 0xae, 0x4d, 0xf1,
  0xca, 0x35, 0xc8, 0xdd, 0x93, 0x1b, 0x58, 0x1e, 0x94, 0x08, 0xcf, 0xa0,
  0x20, 0xb9, 0x75, 0xa5, 0x4c, 0x77, 0xf5, 0x7f, 0xed, 0xd5, 0xcd, 0x53,
  0xaa, 0x87, 0xa6, 0x3c, 0xf5, 0x72, 0xd8, 0xd2, 0xb0, 0xf7, 0x11, 0xb0,
  0x0e, 0xe9, 0x41, 0xd6, 0x8e, 0xd9, 0x07, 0xf8, 0xed, 0xf8, 0x67, 0x7f,
  0x28, 0x18, 0xf0, 0x1b, 0x29, 0x11
};
//unsigned int default_certificate_len = 390;
unsigned int default_certificate_len = sizeof(default_certificate);

unsigned char default_private_key[] = {
  0x30, 0x82, 0x01, 0x3a, 0x02, 0x01, 0x00, 0x02, 0x41, 0x00, 0xb9, 0x83,
  0x30, 0xca, 0xfb, 0xec, 0x11, 0x9e, 0x94, 0xb7, 0x89, 0xf2, 0x84, 0x2c,
  0xda, 0xe1, 0x9a, 0x53, 0x3a, 0x1b, 0x6e, 0xc9, 0x85, 0x81, 0xf9, 0xa3,
  0x41, 0xdb, 0xe2, 0x82, 0x3b, 0xfa, 0x80, 0x22, 0x3b, 0x81, 0x6d, 0x25,
  0x73, 0x7e, 0xf6, 0x49, 0xcc, 0x69, 0x3c, 0x6c, 0xd8, 0x05, 0xfb, 0x92,
  0x02, 0xcf, 0x19, 0x2a, 0x10, 0x7d, 0x69, 0x7a, 0xd8, 0x9d, 0xd3, 0xcf,
  0x6c, 0xef, 0x02, 0x03, 0x01, 0x00, 0x01, 0x02, 0x40, 0x1d, 0x13, 0x92,
  0xf2, 0x3d, 0xca, 0x22, 0x78, 0xd8, 0x96, 0x6b, 0xe8, 0xb7, 0x0e, 0xd0,
  0xbf, 0xcb, 0x90, 0x7f, 0xeb, 0x0c, 0xd2, 0x49, 0x3a, 0xb6, 0x06, 0x00,
  0xac, 0x96, 0x34, 0x13, 0x72, 0x4b, 0x8c, 0xd2, 0xb9, 0x35, 0xf5, 0x64,
  0x18, 0xb2, 0x47, 0x5b, 0x9f, 0xbb, 0xf2, 0x5b, 0x2f, 0x66, 0x78, 0x2d,
  0x0a, 0x76, 0x44, 0xc5, 0x4f, 0xdb, 0x7d, 0x13, 0xcf, 0xa5, 0x08, 0xdc,
  0x01, 0x02, 0x21, 0x00, 0xdf, 0x9a, 0x89, 0xd0, 0xef, 0x23, 0xcf, 0x12,
  0xac, 0x8a, 0x63, 0x1a, 0x8c, 0xc0, 0x3f, 0xf4, 0x38, 0x52, 0x3c, 0x9f,
  0x19, 0x0a, 0x37, 0xd2, 0xcb, 0x5d, 0xeb, 0xb6, 0x2a, 0x33, 0xb0, 0x91,
  0x02, 0x21, 0x00, 0xd4, 0x63, 0xd9, 0x6a, 0x18, 0x5b, 0xe8, 0xa8, 0x57,
  0x4d, 0xd1, 0x9a, 0xa8, 0xd7, 0xe1, 0x65, 0x75, 0xb3, 0xb9, 0x5c, 0x94,
  0x14, 0xca, 0x98, 0x41, 0x47, 0x9c, 0x0a, 0x22, 0x38, 0x05, 0x7f, 0x02,
  0x20, 0x6a, 0xce, 0xfd, 0xef, 0xe0, 0x9b, 0x61, 0x49, 0x91, 0x43, 0x95,
  0x6d, 0x54, 0x38, 0x6d, 0x14, 0x32, 0x67, 0x0d, 0xf0, 0x0d, 0x5c, 0xf5,
  0x27, 0x6a, 0xdf, 0x55, 0x3d, 0xb1, 0xd0, 0xf9, 0x11, 0x02, 0x21, 0x00,
  0xba, 0x94, 0xa0, 0xf9, 0xb0, 0x3e, 0x85, 0x8b, 0xe5, 0x6e, 0x4a, 0x95,
  0x88, 0x80, 0x65, 0xd5, 0x00, 0xea, 0x8b, 0x0b, 0x46, 0x57, 0x61, 0x87,
  0x11, 0xc9, 0xfb, 0xcd, 0x77, 0x34, 0x29, 0xb7, 0x02, 0x20, 0x06, 0x8d,
  0x41, 0x11, 0x47, 0x93, 0xcb, 0xad, 0xda, 0x5d, 0xe1, 0x9d, 0x49, 0x8d,
  0xe0, 0xab, 0x48, 0xe6, 0x18, 0x28, 0x4a, 0x94, 0xae, 0xf9, 0xad, 0xc5,
  0x5b, 0x0b, 0x15, 0xc6, 0x73, 0x17
};
// unsigned int default_private_key_len = 318;
unsigned int default_private_key_len = sizeof(default_private_key);
