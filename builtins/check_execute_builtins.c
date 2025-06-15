/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_execute_builtins.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:06:42 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/15 03:38:31 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	open_out_file(t_redirection *redir,
			int original_stdout, t_shell *shell)
{
	int	fd_out;
	int	flags;

	flags = O_WRONLY | O_CREAT;
	if (redir->append)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	fd_out = open(redir->file, flags, 0666);
	if (fd_out == -1)
	{
		print_error_command(redir->file, "Failed to open file");
		shell->exit_status = 1;
		restore_fds(STDIN_FILENO, original_stdout);
		return (-1);
	}
	dup2(fd_out, STDOUT_FILENO);
	close(fd_out);
	return (0);
}

static int	handle_redirection_loop(t_command_data *data, int i,
		int original_stdout, t_shell *shell)
{
	int	j;

	j = 0;
	while (j < data->num_out_redirs[i])
	{
		if (open_out_file(&data->out_redirs[i][j],
			original_stdout, shell) == -1)
			return (1);
		j++;
	}
	return (0);
}

int	handle_output_redirection(t_command_data *data, int *i,
		int original_stdout, t_shell *shell)
{
	if (data->num_out_redirs && data->num_out_redirs[*i] > 0)
	{
		if (handle_redirection_loop(data, *i, original_stdout, shell) != 0)
			return (1);
	}
	return (0);
}

int	execute_builtin_command(t_command_data *data,
		char **args, t_shell *shell, int *i)
{
	int	result;

	if (ft_strcmp(data->commands[*i], "echo") == 0)
		return (ft_echo(args, shell), 1);
	if (ft_strcmp(data->commands[*i], "pwd") == 0)
	{
		result = ft_pwd();
		if (result == 1)
			shell->exit_status = 0;
		else
			shell->exit_status = 1;
		return (1);
	}
	if (ft_strcmp(data->commands[*i], "env") == 0)
		return (ft_env(args, shell), 1);
	if (ft_strcmp(data->commands[*i], "cd") == 0)
		return (ft_cd(args, i, shell), 1);
	if (ft_strcmp(data->commands[*i], "export") == 0)
		return (ft_export(args, shell), 1);
	if (ft_strcmp(data->commands[*i], "unset") == 0)
		return (ft_unset(args, &shell->vars, &shell->envp), 1);
	if (ft_strcmp(data->commands[*i], "exit") == 0)
		return (ft_exit(args, shell, data), 1);
	return (0);
}

int	child_builtin(int *i, t_shell *shell, t_command_data *data)
{
	int		original_stdin;
	int		original_stdout;
	char	**args;

	original_stdin = dup(STDIN_FILENO);
	original_stdout = dup(STDOUT_FILENO);
	args = data->arguments[*i];
	if (handle_input_redirection(data, i, original_stdin, shell))
		return (1);
	if (handle_output_redirection(data, i, original_stdout, shell))
		return (1);
	execute_builtin_command(data, args, shell, i);
	restore_fds(original_stdin, original_stdout);
	return (1);
}
