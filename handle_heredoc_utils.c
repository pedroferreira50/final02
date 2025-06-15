/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_heredoc_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 11:19:34 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/11 04:56:56 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <termios.h>

// d is data
/// @brief Processes heredoc redirection operator during command parsing
/// @param redir_ctx Context containing args, data, indices,
///shell, and cmd_index
void	handle_heredoc(t_redirect_context	*redir_ctx)
{
	if (validate_heredoc_args(redir_ctx->args,
			redir_ctx->indices, redir_ctx->shell) == -1)
		return ;
	if (redir_ctx->data
		&& redir_ctx->data->heredoc_delims[redir_ctx->cmd_index])
	{
		free(redir_ctx->data->heredoc_delims[redir_ctx->cmd_index]);
		redir_ctx->data->heredoc_delims[redir_ctx->cmd_index] = NULL;
	}
	set_heredoc_delimiter(redir_ctx->data,
		redir_ctx->args[redir_ctx->indices->i + 1], redir_ctx->cmd_index);
	if (redir_ctx->data->heredoc_delims[redir_ctx->cmd_index] == NULL)
	{
		redir_ctx->shell->exit_status = 1;
		redir_ctx->indices->i += 2;
		return ;
	}
	redir_ctx->data->heredoc_fds[redir_ctx->cmd_index]
		= setup_heredoc(redir_ctx->data->heredoc_delims[redir_ctx->cmd_index],
			redir_ctx->shell,
			redir_ctx->data->heredoc_quoted[redir_ctx->cmd_index]);
	redir_ctx->indices->i += 2;
}

/// @brief Prints bash-compatible warning when heredoc reaches EOF
/// without delimiter
/// @param delimiter Expected heredoc delimiter that wasn't found
void	print_heredoc_eof_warning(const char *delimiter)
{
	if (!delimiter)
		delimiter = "";
	ft_putstr_fd("\n", 2);
	ft_putstr_fd("minishell: warning: here-document ", 2);
	ft_putstr_fd("delimited by end-of-file (wanted `", 2);
	ft_putstr_fd((char *)delimiter, 2);
	ft_putstr_fd("')", 2);
	ft_putstr_fd("\n", 2);
}

/// @brief Configures terminal to suppress SIGQUIT visual
/// echo during heredoc
/// @param tty_fd Terminal file descriptor (typically STDIN_FILENO)
/// @param old_termios Pointer to store original terminal settings
/// for restoration
void	prepare_heredoc_terminal(int tty_fd, struct termios *old_termios)
{
	struct termios	new_termios;

	tcgetattr(tty_fd, old_termios);
	new_termios = *old_termios;
	new_termios.c_lflag &= ~ECHOCTL;
	tcsetattr(tty_fd, TCSANOW, &new_termios);
}

/// @brief Restores terminal settings and performs heredoc cleanup
/// @param ctx Heredoc context with terminal settings and pipe info
/// @param code Exit code to pass to cleanup_heredoc
/// @param line Line buffer to pass to cleanup_heredoc (may be NULL)
/// @return Result from cleanup_heredoc function
static int	restore_and_cleanup(t_heredoc_context *ctx, int code, char *line)
{
	if (isatty(STDIN_FILENO))
		tcsetattr(STDIN_FILENO, TCSANOW, ctx->old_termios);
	return (cleanup_heredoc(ctx->pipefd, code, ctx->shell, line));
}

/// @brief Main heredoc input loop - reads lines until delimiter or signal
/// @param delimiter String to match for termination
/// @param suppress_expansion 1=quoted delimiter (no expansion),
/// 0=expand variables
/// @param ctx Heredoc context with shell, pipe, and terminal info
/// @return Read end fd on success, -1 on error/interrupt
int	process_heredoc_loop(const char *delimiter, int suppress_expansion,
	t_heredoc_context *ctx)
{
	char	*line;

	while (1)
	{
		if (g_signal == SIGINT)
			return (restore_and_cleanup(ctx, 130, NULL));
		line = read_heredoc_line();
		if (g_signal == SIGINT)
			return (restore_and_cleanup(ctx, 130, line));
		if (line == NULL)
		{
			if (isatty(STDIN_FILENO))
				print_heredoc_eof_warning(delimiter);
			return (restore_and_cleanup(ctx, 1, NULL));
		}
		if (ft_strcmp(line, delimiter) == 0)
			return (restore_and_cleanup(ctx, 0, line));
		write_heredoc_line(ctx->pipefd[1], line,
			suppress_expansion, ctx->shell);
		free(line);
	}
}
