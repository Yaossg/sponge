#include "socket.hh"
#include "util.hh"

#include <cstdlib>
#include <iostream>

using namespace std;

void get_URL(const string &host, const string &path) {
    Address address(host, "http");
    TCPSocket socket;
    socket.connect(address);
    string HTTP_request = "GET ";
    HTTP_request += path;
    HTTP_request += " HTTP/1.1\r\n";
    HTTP_request += "Host: ";
    HTTP_request += host;
    HTTP_request += "\r\n";
    HTTP_request += "Connection: close";
    HTTP_request += "\r\n\r\n";
    socket.write(HTTP_request);
    while (!socket.eof()) {
        string upcoming = socket.read();
        cout << upcoming;
    }
}

int main(int argc, char *argv[]) {
    try {
        if (argc <= 0) {
            abort();  // For sticklers: don't try to access argv[0] if argc <= 0.
        }

        // The program takes two command-line arguments: the hostname and "path" part of the URL.
        // Print the usage message unless there are these two arguments (plus the program name
        // itself, so arg count = 3 in total).
        if (argc != 3) {
            cerr << "Usage: " << argv[0] << " HOST PATH\n";
            cerr << "\tExample: " << argv[0] << " stanford.edu /class/cs144\n";
            return EXIT_FAILURE;
        }

        // Get the command-line arguments.
        const string host = argv[1];
        const string path = argv[2];

        // Call the student-written function.
        get_URL(host, path);
    } catch (const exception &e) {
        cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
