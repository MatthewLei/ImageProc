//written by: Matthew Lei
//png chunk specifications: https://www.w3.org/TR/PNG-Chunks.html

#include <iostream>
#include <fstream>

using namespace std;

static const int PNG_SIG_SZ = 8;
static const int PNG_IHDR_SZ = 13;
static const int PNG_CLR_DEPTH = 5;
static const int PNG_SIG[PNG_SIG_SZ] = {137, 80, 78, 71, 13, 10, 26, 10};
static const int BIT_DEPTH[PNG_CLR_DEPTH] = {1, 2, 4, 8, 16};

enum color_t {
    GS,                      /// grayscale
    RGB = 2,                 /// RGB triplet
    PLTE_IDX,                /// palette index
    GS_A,                    /// grayscale w/ alpha
    RGB_A = 6                /// RGB w/ alpha
};

struct ImageHeader {
    unsigned int width;      /// 4 bytes, width of image
    unsigned int height;     /// 4 bytes, height of image
    unsigned char bit_depth;        /// 1 byte, bits per sample (1,2,4,8,16)
    unsigned char color_type;       /// 1 byte, see specification (0,2,3,4,6)
    unsigned char compression;      /// 1 byte, compresssion method (0)
    unsigned char filter;           /// 1 byte, filter method (0)
    unsigned char interlace;        /// 1 byte, interlace method (0)
};

/*********************** PROTOTYPES ******************************/



int find_idat(ifstream &fs);
bool is_png_ext(string input);
void png_get_idat(ifstream &fs);
ImageHeader png_get_ihdr(ifstream &fs);
void print_bits(char b);
void print_ihdr(ImageHeader &ihdr);
unsigned int read_uint(ifstream &fs);
void scan_bytes(ifstream &fs, int num);
bool png_check_sig(ifstream &fs);

/*********************** FUNCTIONS *******************************/

/**
 *  is_png_ext - checks for PNG extension
 *  @param input User input string
 *  @return True if input has PNG extension
 */
bool is_png_ext(string input) {
    const int TERM_CHAR = 1; //1 byte for terminating \0
    int len = input.size();
    int ext_len = sizeof(".png") - TERM_CHAR;
    if (len < ext_len || input.find(".png", len - ext_len) == string::npos) {
	cout << input << " does not have .png extension\n";
	return false;
    } else {
	return true;
    }
}

void get_file_name(string &fileName) {
    cout << "PNG file name: ";
    cin >> fileName;
    while (!is_png_ext(fileName)) {
	cout << fileName << " does not have .png extension. Try again: ";
	cin >> fileName;
    }
}

/** @brief Checks for PNG file signiture
 *
 *  Checks first 8 bytes of the file for PNG
 *  file signature according to PNG specifications.
 *  sig (dec): 137, 80, 78, 71, 13, 10, 26, 10
 *  sig (ascii): \211 P N G \r \n \032 \n
 *  ref: http://www.libpng.org/pub/png/spec/1.2/PNG-Structure.html
 *
 *  @param fs File stream
 *  @return True if file has valid PNG signature
 */
bool png_check_sig(ifstream &fs) {
    char byte;
    unsigned char ubyte;
    int cur;

    cur = fs.tellg();
    fs.seekg(fs.beg);
    for (int i = 0; i < PNG_SIG_SZ; ++i) {
	fs.get(byte);
	ubyte = (unsigned char) byte;
	if ((int) ubyte != PNG_SIG[i]) {
	    cout << "file is not a PNG image or corrupted.\n";
	    return false;
	}
    }
    fs.seekg(cur, fs.beg);
    return true;
}

void print_bits(char b) {
    bitset<8> bits(b);
    cout << bits << " ";
}

void print_ihdr(ImageHeader &ihdr) {
    cout << "width: " << ihdr.width << endl;
    cout << "height: " << ihdr.height << endl;
    cout << "bit depth: " << (int) ihdr.bit_depth << endl;
    cout << "color type: " << (int) ihdr.color_type << endl;
    cout << "compression method: " << (int) ihdr.compression << endl;
    cout << "filter method: " << (int) ihdr.filter << endl;
    cout << "interlace method: " << (int) ihdr.interlace << endl;
    cout << endl;
}

unsigned int read_uint(ifstream &fs) {
    char b1, b2, b3, b4;
    unsigned char ub1, ub2, ub3, ub4;

    fs.get(b4);
    fs.get(b3);
    fs.get(b2);
    fs.get(b1);

    ub4 = (unsigned char) b4;
    ub3 = (unsigned char) b3;
    ub2 = (unsigned char) b2;
    ub1 = (unsigned char) b1;

    unsigned int val = (ub4 << 24) | (ub3 << 16) | (ub2 << 8) | ub1;
    return val;
}

///find beginning of idat byte location
int find_idat(ifstream &fs) {
    char b1;
    char str[] = {'I', 'D', 'A', 'T'};
    fs.seekg(PNG_IHDR_SZ, fs.beg);

    while (fs.get(b1)) {
	for (int i = 0; i < 4; i++) {
	    if (b1 == str[i]) {
		
	    }
	}
    }
}

void png_get_idat(ifstream &fs) {
    unsigned int chunk_length;
    unsigned int chunk_type;

    //    scan_bytes(fs,4);

    chunk_length = read_uint(fs);
    chunk_type = read_uint(fs);

    cout << "chunk length: " << chunk_length << endl;
    cout << "chunk type: " << chunk_type << endl;
}

/** @brief Get IHDR data chunk from PNG file stream
 *  @param fs File stream
 *  @return ImageHeader struct with IHDR data
 *
 *  Note: function will restore read cursor to
 *        original location before function call
 */
ImageHeader png_get_ihdr(ifstream &fs) {
    char byte;
    int cur;
    ImageHeader ihdr;

    cur = fs.tellg();

    /// skip file signature
    fs.seekg(PNG_SIG_SZ, fs.beg);

    /// NUL, NUl, NUL, CR
    fs.seekg(4, fs.cur);

    /// 'I', 'H', 'D', 'R'
    fs.seekg(4, fs.cur);

    ihdr.width = read_uint(fs);
    ihdr.height = read_uint(fs);

    fs.get(byte);
    ihdr.bit_depth = (unsigned char) byte;

    fs.get(byte);
    ihdr.color_type = (unsigned char) byte;

    fs.get(byte);
    ihdr.compression = (unsigned char) byte;
    if (ihdr.compression != 0) {
	cerr << "BAD_BYTE: invalid compression method\n";
	exit(0);
    }

    fs.get(byte);
    ihdr.filter = (unsigned char) byte;
    if (ihdr.filter != 0) {
	cerr << "BAD_BYTE: invalid filter method\n";
	exit(0);
    }

    fs.get(byte);
    ihdr.interlace = (unsigned char) byte;
    if (ihdr.interlace != 0) {
	cerr << "BAD_BYTE: invalid interlace method\n";
	exit(0);
    }

    fs.seekg(cur, fs.beg);
    return ihdr;
}

///debugging function to scan next few bytes for readable message
void scan_bytes(ifstream &fs, int num) {
    char byte;
    unsigned char ubyte;
    unsigned int ui;
    for (int i = 0; i < num; ++i) {
	fs.get(byte);
	ubyte = (unsigned char) byte;
	fs.seekg(-1, fs.cur);
	ui = read_uint(fs);

	cout << "byte as char: " << byte << endl;
	cout.flush();
	cout << "byte as ascii: " << (int) ubyte << endl;
	cout.flush();
	cout << "byte as bits: ";
	print_bits(byte);
	cout << endl;
	cout << "next 4 bytes as unsigned int: " << ui << endl << endl;

	fs.seekg(-3, fs.cur);
    }
}

/***************************************************************************/

int main(int argc, char** argv) {
    string fileName;
    ifstream fs;
    if (argc > 2) {
	/// incorrect usage
	cout << "usage: ./prog <img_name.png>\n";
	exit(0);
    } else if (argc > 1) {
	/// command line arguments
	if (is_png_ext(string(argv[1]))) {
	    fileName = string(argv[1]);
	} else {
	    exit(0);
	}
    } else {
	/// prompt for file name
	get_file_name(fileName);
    }

    fs.open(fileName);
    while (!fs.good()) {
	cout << "File does not exist, try again.\n";
	get_file_name(fileName);
	fs.open(fileName);
    }

    /// check and skip file signature
    png_check_sig(fs);
    fs.seekg(PNG_SIG_SZ, fs.beg);

    /// get and skip image header
    ImageHeader ihdr = png_get_ihdr(fs);
    fs.seekg(4, fs.cur);   /// NUL NUL NUL CR
    fs.seekg(4, fs.cur);   /// I H D R
    fs.seekg(PNG_IHDR_SZ, fs.cur);

    print_ihdr(ihdr);

    //int npixels = ihdr.width * ihdr.height;

    switch (ihdr.color_type) {
    case (GS):
	break;
    case (RGB):
	break;
    case (PLTE_IDX):
	cout << "Color palette chunk not implemented yet\n";
	exit(0);
	break;
    case (GS_A):
	break;
    case(RGB_A):
	png_get_idat(fs);
	break;
    }
}
