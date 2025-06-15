/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 11:27:01 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/11 04:51:45 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/// @brief Sets up input redirection for command (heredoc takes
/// precedence over file)
/// @param data Command data with heredoc_fds and input_files arrays
/// @param i Current command index in pipeline
/// @param shell Global shell state for error handling
static void	setup_input_redirect(t_command_data *data, int i, t_shell *shell)
{
	int	fd;

	fd = -1;
	if (data->heredoc_fds && data->heredoc_fds[i] != -1)
	{
		dup2(data->heredoc_fds[i], STDIN_FILENO);
		close(data->heredoc_fds[i]);
		data->heredoc_fds[i] = -1;
	}
	else if (data->input_files && data->input_files[i] != NULL)
	{
		fd = open(data->input_files[i], O_RDONLY);
		if (fd < 0)
		{
			shell->exit_status = 1;
			exit(1);
		}
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
}

static void	redirect_and_close(int oldfd, int newfd)
{
	dup2(oldfd, newfd);
	close(oldfd);
}

static int	setup_output_redirect(t_command_data *data, int i, t_shell *shell)
{
	int	fd;
	int	flags;
	int	j;

	j = 0;
	if (data->num_out_redirs && data->num_out_redirs[i] > 0)
	{
		while (j < data->num_out_redirs[i])
		{
			flags = O_WRONLY | O_CREAT;
			if (data->out_redirs[i][j].append)
				flags |= O_APPEND;
			else
				flags |= O_TRUNC;
			fd = open(data->out_redirs[i][j].file, flags, 0666);
			if (fd < 0)
			{
				shell->exit_status = 1;
				return (-1);
			}
			redirect_and_close(fd, STDOUT_FILENO);
			j++;
		}
	}
	return (0);
}

void	setup_pipes_and_redirections(t_command_data *data, t_exec_state *state,
		int num_commands, t_shell *shell)
{
	int	has_heredoc;

	has_heredoc = (data->heredoc_fds && data->heredoc_fds[state->i] != -1);
	if (state->i > 0 && !has_heredoc)
	{
		dup2(state->prev_pipe_read, STDIN_FILENO);
		close(state->prev_pipe_read);
	}
	else if (state->i > 0 && has_heredoc)
	{
		close(state->prev_pipe_read);
	}
	setup_input_redirect(data, state->i, shell);
	if (state->i < num_commands - 1)
	{
		dup2(state->pipefd[1], STDOUT_FILENO);
	}
	if (setup_output_redirect(data, state->i, shell) < 0)
		exit(shell->exit_status);
}

int	setup_pipeline(t_command_data *data, t_exec_state *state, t_shell *shell)
{
	int	result;

	result = 0;
	if (state->i < data->num_commands - 1)
	{
		result = pipe(state->pipefd);
		if (result == -1)
		{
			perror("minishell: pipe");
			shell->exit_status = 1;
			return (-1);
		}
	}
	return (0);
}
