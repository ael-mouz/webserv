// #include <iostream>
// #include <sys/stat.h>
// #include <ctime>

// int main() {
//     const char* filename = "./config/config.conf"; // Replace with the path to your file.

//     struct stat fileStat;

//     if (stat(filename, &fileStat) == 0) {
//         std::cout << "File Information:" << std::endl;
//         std::cout << "Size: " << fileStat.st_size << " bytes" << std::endl;
//         std::cout << "Permissions: " << (fileStat.st_mode & 0777) << std::endl;
//         std::cout << "User ID of owner: " << fileStat.st_uid << std::endl;
//         std::cout << "Group ID of owner: " << fileStat.st_gid << std::endl;
//         std::cout << "Device ID (if special file): " << fileStat.st_dev << std::endl;
//         std::cout << "Inode number: " << fileStat.st_ino << std::endl;

//         // Access time (last access of the file)
//         std::cout << "Last access time: " << std::ctime(&fileStat.st_atime);

//         // Modification time (last modification of the file)
//         std::cout << "Last modification time: " << std::ctime(&fileStat.st_mtime);

//         // Change time (last change of file status)
//         std::cout << "Last status change time: " << std::ctime(&fileStat.st_ctime);

//         // You can extract more information using other fields as needed.
//     } else {
//         std::cerr << "Failed to get file information." << std::endl;
//     }

//     return 0;
// }




#include <iostream>
#include <dirent.h>

int main()
{
	const char *directory_path = "./config/"; // Replace with the path to your directory.

	DIR *dir = opendir(directory_path);

	if (dir)
	{
		std::cout << "Contents of the directory '" << directory_path << "':" << std::endl;

		struct dirent *entry;
		while ((entry = readdir(dir)) != nullptr)
		{
			std::cout <<entry->d_seekoff << "+++++++++++"<<entry->d_namlen << std::endl;
			std::cout << "Name: " << entry->d_name << std::endl;
			std::cout << "Inode number: " << entry->d_ino << std::endl;
			std::cout << "Entry type: " << (entry->d_type == DT_REG ? "Regular File" : (entry->d_type == DT_DIR ? "Directory" : "Other")) << std::endl;
		}

		closedir(dir);
	}
	else
	{
		std::cerr << "Failed to open the directory." << std::endl;
	}

	return 0;
}




// #include <iostream>
// #include <cstdlib>
// #include <cstring>
// #include <unistd.h>

// int main() {
//     const char* path = "./config/"; // Replace with the path to your file or directory.
    
//     // Check if the file or directory is readable for the current user.
//     if (access(path, R_OK) == 0) {
//         std::cout << path << " is readable." << std::endl;
//     } else {
//         std::cerr << path << " is not readable." << std::endl;
//     }
    
//     // Check if the file or directory is writable for the current user.
//     if (access(path, W_OK) == 0) {
//         std::cout << path << " is writable." << std::endl;
//     } else {
//         std::cerr << path << " is not writable." << std::endl;
//     }
    
//     // Check if the file or directory is executable for the current user.
//     if (access(path, X_OK) == 0) {
//         std::cout << path << " is executable." << std::endl;
//     } else {
//         std::cerr << path << " is not executable." << std::endl;
//     }
    
//     // Check if the file or directory exists.
//     if (access(path, F_OK) == 0) {
//         std::cout << path << " exists." << std::endl;
//     } else {
//         std::cerr << path << " does not exist." << std::endl;
//     }
    
//     return 0;
// }
