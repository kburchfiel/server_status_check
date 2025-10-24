/* Simple script for keeping track of a server's uptime
By Kenneth Burchfiel
Released under the MIT license

This script will:
1. Calculate the current time
2. Update a local laptop uptime log
3. Attempt to download a 'latest_uptime.txt' file from a server
4. (If this download is successful) update a local server uptime
log; create a new copy of this latest_uptime.txt file (that will 
store the current time calculated earlier); and push the 
new latest_uptime.txt file back to the server

(Note: an earlier version of this script uploaded the server and 
laptop uptime logs to the server as well; however, as these files
grow in size, this would quickly use up a great deal of bandwidth.
The current approach, which only downloads and uploads a 25-byte
file, is far less resource intensive.)

Prerequisites:
1. Make sure that a 'server_uptime_folder' is present within
your server's file system at the desired path, and that 
a 'latest_uptime.txt' file is present within this folder. (The
file can be empty.)
2. Update paths/directories as needed.
*/

#include <iostream>
#include <chrono>
#include <filesystem>
#include <fstream>


int main()
{
     auto start_time = std::chrono::high_resolution_clock::now();
  // This code was based on
  // https://en.cppreference.com/w/cpp/chrono/system_clock/to_time_t.html
  // and https://en.cppreference.com/w/cpp/chrono/system_clock/now.html .

std::time_t current_time = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());

// Creating a string version of this timestamp that shows
// the user's local time:
// The following formatting code was based on the examples
// at
// https://en.cppreference.com/w/cpp/chrono/c/strftime
// I could also have used std::format(), but
// this isn't available within older C++ implementations.
// (See https://en.cppreference.com/w/cpp/chrono/
// system_clock/formatter) for more information on that option.)
char current_time_strftime_container[25];
std::strftime(current_time_strftime_container, 
    25, "%FT%T%z",
    std::localtime(&current_time));
std::string current_time_string = current_time_strftime_container;

// Updating (or creating, if not already present) a local file
// that keeps track of the uptime of the laptop on which the 
// server is running:
// (This code will still work even if the NextCloud server
// isn't accessible,
// so it will help identify times when the laptop is running
// but the server is not. I placed it above the server-specific code
// so that it would still run even if the rest of the program
// failed to execute.)


// We don't need to see the other contents of this file; therefore,
// we can use the 'app' mode to write to it, which should be more
// efficient than attempting to all of it into memory beforehand.


std::ofstream laptop_log_ofs(
    "../local_uptime_folder/laptop_uptime_log.txt",
std::ofstream::app);
laptop_log_ofs << current_time_string << "\n";
laptop_log_ofs.close();

// Deleting the local latest_uptime.txt file (if it exists):
// (This step will play an important role later on, when the program
// will use the presence of this file as a sign that it was able
// to successfully connect to the server.)

// This code is based on
// https://en.cppreference.com/w/cpp/filesystem/remove.html .
// I initially tried to use remove(), but it raised an error because
// files were present; this same error didn't get raised with
// remove_all().
std::filesystem::remove("../local_uptime_folder/latest_uptime.txt");

// Attempting to retrieve the server's copy of the 
// latest_uptime.txt file:

try {
// Local-filesystem version for debugging:
// std::filesystem::copy("/home/kjb3/D1V1/kjb3docs\
// /Datasets/server_uptime_folder/latest_uptime.txt", 
// "../local_uptime_folder/latest_uptime.txt",
// std::filesystem::copy_options::overwrite_existing);

// The following code was based on the examples shown at
// https://www.geeksforgeeks.org/cpp/system-call-in-c/ 
// and https://rclone.org/commands/rclone_copyto/ .
std::system("rclone copyto nxc_admin:'/Admin and \
Ken share/server_uptime_folder/latest_uptime.txt' \
'../local_uptime_folder/latest_uptime.txt' --verbose");
}

catch (...)

{
    std::cout << "Error during file retrieval attempt.\n";
}

// Because we deleted our local copy of latest_uptime.txt before
// running the above code, we can conclude that, if this file
// is now present within our directory, we did indeed successfully
// connect to the server. Otherwise, we'll assume that the server
// is down.

// The following code is based on:
// https://en.cppreference.com/w/cpp/filesystem/exists.html

if (std::filesystem::exists(
    "../local_uptime_folder/latest_uptime.txt"))

{
    std::cout << "Successfully connected to server.\n";
// Writing the current time to the server-specific uptime log:
// This code is based on
// https://en.cppreference.com/w/cpp/io/basic_ofstream.html
// and 
// https://en.cppreference.com/w/cpp/io/ios_base/openmode

std::ofstream server_log_ofs(
"../local_uptime_folder/server_uptime_log.txt",
std::ofstream::app);
server_log_ofs << current_time_string << "\n";
server_log_ofs.close();

// Updating latest_uptime.txt:
// (Since this file contains only the latest uptime, copying
// it back to the server, even on a frequent basis (e.g. every minute)
// will incur much less bandwidth than would copying
// server_uptime_log.txt.

// (Any existing uptime will get overwritten by this command.)
std::ofstream latest_ofs(
    "../local_uptime_folder/latest_uptime.txt");
latest_ofs << current_time_string << "\n";
// Note: When I didn't explicitly close my ofstreams, I found that
// zero-byte files were being copied to the server folder. I figured
// that this was because the data might not have been saved to 
// the file just yet. Closing the ofstreams resolved this issue.
latest_ofs.close(); // from p. 256 of Programming: Principles and
// Practice Using C++ (3rd Edition)

// Copying latest_uptime.txt back to the server:

try {
// Local-filesystem version for debugging:
// std::filesystem::copy(
//     "../local_uptime_folder/latest_uptime.txt", 
//     "/home/kjb3/D1V1/kjb3docs/Datasets/\
// server_uptime_folder/latest_uptime.txt", 
// std::filesystem::copy_options::overwrite_existing);

// The following code was based on the examples shown at
// https://www.geeksforgeeks.org/cpp/system-call-in-c/ 
// and https://rclone.org/commands/rclone_copyto/ .
std::system(
"rclone copyto '../local_uptime_folder/latest_uptime.txt' \
nxc_admin:'/Admin and Ken \
share/server_uptime_folder/latest_uptime.txt' --verbose");
}

catch (...)

{
    std::cout << "Error during file upload attempt.\n";
}


}
else 
{std::cout << "Failed to connect to server.\n";}


auto end_time = std::chrono::high_resolution_clock::now();

// (The following code was based in part on the examples found at
  // https://en.cppreference.com/w/cpp/chrono/duration .)

double run_seconds =
    std::chrono::duration<double>(end_time - start_time).count();


std::cout << "Finished running script in " << 
run_seconds << " seconds.\n";


}