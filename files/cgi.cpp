/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/15 12:13:57 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/15 12:17:49 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <map>


int executePerlCGI(const std::string &scriptPath, const std::multimap<std::string, std::string> &headers, const std::string &body)
{
	int pipefd[2];
	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return -1;
	}
	pid_t pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return -1;
	}
	else if (pid == 0)
	{
		close(pipefd[0]);
		setenv("REQUEST_METHOD", "POST", 1);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		execl("/usr/bin/perl", "perl", scriptPath.c_str(), NULL);
		perror("execl");
		exit(EXIT_FAILURE);
	}
	else
	{
		close(pipefd[1]);
		char buffer[1024];
		ssize_t bytesRead;
		while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0)
			fwrite(buffer, 1, bytesRead, stdout);
		int status;
		if (waitpid(pid, &status, 0) == -1)
			perror("waitpid");
		if (WIFEXITED(status))
		{
			int exitStatus = WEXITSTATUS(status);
			if (exitStatus == 0)
			{
				// CGI script executed successfully
			}
			else
			{
				// Handle CGI script failure
				std::cerr << "CGI script failed with exit code: " << exitStatus << std::endl;
			}
		}
		else if (WIFSIGNALED(status))
		{
			// Handle termination by a signal
			std::cerr << "CGI script terminated by signal: " << WTERMSIG(status) << std::endl;
		}
	}

	return 0;
}

int main()
{
	std::string scriptPath = "your_perl_script.pl"; // Replace with your script path
	std::multimap<std::string, std::string> headers;
	// Add CGI request headers to the 'headers' multimap
	std::string body = "Your request body here";

	// Execute the Perl CGI script
	int result = executePerlCGI(scriptPath, headers, body);

	return result;
}
