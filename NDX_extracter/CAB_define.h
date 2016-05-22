#define CAB_SIGNATURE        SWAPDWORD ('MSCF')
#define CAB_VERSION          0x0103

#define CAB_FLAG_HASPREV     0x0001
#define CAB_FLAG_HASNEXT     0x0002
#define CAB_FLAG_RESERVE     0x0004

#define CAB_ATTRIB_READONLY  0x0001
#define CAB_ATTRIB_HIDDEN    0x0002
#define CAB_ATTRIB_SYSTEM    0x0004
#define CAB_ATTRIB_VOLUME    0x0008
#define CAB_ATTRIB_DIRECTORY 0x0010
#define CAB_ATTRIB_ARCHIVE   0x0020

#define CAB_FILE_FIRST       0x0000
#define CAB_FILE_NEXT        0x0001
#define CAB_FILE_SPLIT       0xFFFE
#define CAB_FILE_CONTINUED   0xFFFD

#define CAB_NOTIFY_OK        1
#define CAB_NOTIFY_ERROR     0
#define CAB_NOTIFY_SKIP      0
#define CAB_NOTIFY_ABORT     (-1)

#pragma pack(1)
typedef struct _CAB_HEADER
{
	DWORD sig;              // file signature 'MSCF' (CAB_SIGNATURE)
	DWORD csumHeader;       // header checksum (0 if not used)
	DWORD cbCabinet;        // cabinet file size
	DWORD csumFolders;      // folders checksum (0 if not used)
	DWORD coffFiles;        // offset of first CAB_ENTRY
	DWORD csumFiles;        // files checksum (0 if not used)
	WORD  version;          // cabinet version (CAB_VERSION)
	WORD  cFolders;         // number of folders
	WORD  cFiles;           // number of files
	WORD  flags;            // cabinet flags (CAB_FLAG_*)
	WORD  setID;            // cabinet set id
	WORD  iCabinet;         // zero-based cabinet number
}
CAB_HEADER, *PCAB_HEADER;

typedef struct _CAB_ENTRY
{
	DWORD cbFile;           // uncompressed file size
	DWORD uoffFolderStart;  // file offset after decompression
	WORD  iFolder;          // file control id (CAB_FILE_*)
	WORD  date;             // file date stamp, as used by DOS
	WORD  time;             // file time stamp, as used by DOS
	WORD  attribs;          // file attributes (CAB_ATTRIB_*)
}
CAB_ENTRY, *PCAB_ENTRY;
#pragma pack()