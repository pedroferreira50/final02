/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_handling.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 11:19:30 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/11 13:14:51 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	free_out_redirs_loop(t_redirection **out_redirs, \
			int *num_out_redirs, int num_cmds)
{
	int	i;
	int	j;

	i = 0;
	while (i < num_cmds)
	{
		j = 0;
		if (out_redirs[i])
		{
			while (j < num_out_redirs[i])
			{
				if (out_redirs[i][j].file)
					free(out_redirs[i][j].file);
				j++;
			}
			free(out_redirs[i]);
		}
		i++;
	}
}

static void	free_out_redirs(t_command_data *data)
{
	if (data->out_redirs)
	{
		free_out_redirs_loop(data->out_redirs, data->num_out_redirs,
			data->num_commands);
		free(data->out_redirs);
		data->out_redirs = NULL;
	}
	if (data->num_out_redirs)
	{
		free(data->num_out_redirs);
		data->num_out_redirs = NULL;
	}
}

static void	free_heredoc_delims(t_command_data *data)
{
	int	i;

	if (data->heredoc_delims)
	{
		i = 0;
		while (i < data->num_commands)
		{
			if (data->heredoc_delims[i])
			{
				free(data->heredoc_delims[i]);
				data->heredoc_delims[i] = NULL;
			}
			i++;
		}
		free(data->heredoc_delims);
		data->heredoc_delims = NULL;
	}
}

static void	free_heredoc_fds_and_quoted(t_command_data *data)
{
	int	i;

	if (data->heredoc_fds)
	{
		i = 0;
		while (i < data->num_commands)
		{
			if (data->heredoc_fds[i] != -1)
			{
				close(data->heredoc_fds[i]);
				data->heredoc_fds[i] = -1;
			}
			i++;
		}
		free(data->heredoc_fds);
		data->heredoc_fds = NULL;
	}
	if (data->heredoc_quoted)
	{
		free(data->heredoc_quoted);
		data->heredoc_quoted = NULL;
	}
}

/// @brief Frees all memory allocated for command data structure
/// @param data Command data structure to free (can be NULL)
void	free_command_data(t_command_data *data)
{
	if (!data)
		return ;
	if (data->commands)
		free_data_commands(data->commands, data->num_commands);
	if (data->arguments)
		free_data_arguments(data->arguments, data->num_commands);
	if (data->input_files)
		free_input_files(data);
	if (data->heredoc_delims)
		free_heredoc_delims(data);
	if (data->heredoc_fds || data->heredoc_quoted)
		free_heredoc_fds_and_quoted(data);
	if (data->out_redirs)
		free_out_redirs(data);
	data->num_commands = 0;
	data->num_pipes = 0;
}
