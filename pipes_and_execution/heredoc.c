/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 11:40:55 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/11 04:52:27 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <termios.h>

/// @brief Creates pipe for heredoc content communication
/// @param pipefd Array to store pipe file descriptors [read_end, write_end]
/// @return 0 on success, -1 on pipe creation failure
static int	init_heredoc_pipe(int *pipefd)
{
	int	result;

	result = pipe(pipefd);
	if (result == -1)
	{
		perror("pipe");
		return (-1);
	}
	return (0);
}

/// @brief Reads single line from heredoc input with signal handling
/// @return Line string with newline removed, NULL on EOF or signal interrupt
char	*read_heredoc_line(void)
{
	char	*line;
	size_t	len;

	if (g_signal == SIGINT)
		return (NULL);
	if (isatty(STDIN_FILENO))
		write(1, "> ", 2);
	line = get_next_line(STDIN_FILENO);
	if (g_signal == SIGINT)
	{
		if (line)
			free(line);
		return (NULL);
	}
	if (line)
	{
		len = ft_strlen(line);
		if (len > 0 && line[len - 1] == '\n')
			line[len - 1] = '\0';
	}
	return (line);
}

/// @brief Processes and writes heredoc line to pipe with
/// optional variable expansion
/// @param pipefd Write end of heredoc pipe
/// @param line Input line to process
/// @param suppress_expansion 1=no expansion (quoted delimiter),
/// 0=expand variables
/// @param shell Global shell state for variable expansion
void	write_heredoc_line(int pipefd, char *line,
				int suppress_expansion, t_shell *shell)
{
	char	*expanded_line;

	expanded_line = NULL;
	if (suppress_expansion != 0)
	{
		expanded_line = ft_strdup(line);
	}
	else
	{
		expanded_line = expand_variables(line, '\0', shell);
	}
	if (expanded_line != NULL)
	{
		write(pipefd, expanded_line, ft_strlen(expanded_line));
		write(pipefd, "\n", 1);
		free(expanded_line);
	}
}

/// @brief Cleans up heredoc resources and sets exit status
/// @param pipefd Pipe file descriptors to close
/// @param exit_code Exit status to set (0=success, 130=SIGINT, 1=EOF)
/// @param shell Global shell state for exit status setting
/// @param line Line buffer to free (may be NULL)
/// @return Read end fd on success, -1 on error/interrupt
int	cleanup_heredoc(int *pipefd, int exit_code,
			t_shell *shell, char *line)
{
	if (line)
		free(line);
	if (exit_code != 0)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		shell->exit_status = exit_code;
		g_signal = 0;
		set_signals_interactive();
		return (-1);
	}
	close(pipefd[1]);
	g_signal = 0;
	set_signals_interactive();
	return (pipefd[0]);
}

/// @brief Main heredoc processing function -
/// reads input until delimiter match
/// @param delimiter String to match for heredoc termination
/// @param shell Global shell state for variable expansion and exit status
/// @param suppress_expansion 1=quoted delimiter
/// (no expansion), 0=unquoted (expand)
/// @return Read end file descriptor for heredoc content, -1 on error/interrupt
int	setup_heredoc(const char *delimiter, t_shell *shell, int suppress_expansion)
{
	int					pipefd[2];
	struct termios		old_termios;
	t_heredoc_context	ctx;

	if (delimiter == NULL || init_heredoc_pipe(pipefd) == -1)
		return (-1);
	g_signal = 0;
	set_signals_simple(0);
	if (isatty(STDIN_FILENO))
		prepare_heredoc_terminal(STDIN_FILENO, &old_termios);
	ctx.shell = shell;
	ctx.pipefd = pipefd;
	ctx.old_termios = &old_termios;
	return (process_heredoc_loop(delimiter, suppress_expansion, &ctx));
}
