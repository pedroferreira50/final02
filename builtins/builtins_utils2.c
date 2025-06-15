/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 11:29:59 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/04 11:38:02 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	restore_fds(int original_stdin, int original_stdout)
{
	dup2(original_stdin, STDIN_FILENO);
	dup2(original_stdout, STDOUT_FILENO);
	close(original_stdin);
	close(original_stdout);
}

int	handle_input_redirection(t_command_data *data, int *i, int original_stdin,
		t_shell *shell)
{
	int	fd_in;

	fd_in = -1;
	if (data->input_files && data->input_files[*i])
	{
		fd_in = open(data->input_files[*i], O_RDONLY);
		if (fd_in == -1)
		{
			perror("open input file");
			shell->exit_status = 1;
			restore_fds(original_stdin, STDOUT_FILENO);
			return (1);
		}
		dup2(fd_in, STDIN_FILENO);
		close(fd_in);
	}
	return (0);
}
