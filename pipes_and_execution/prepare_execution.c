/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prepare_execution.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 03:41:16 by pviegas-          #+#    #+#             */
/*   Updated: 2025/06/15 03:49:54 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	set_default_true_command(t_command_data *data, int i)
{
	data->commands[i] = ft_strdup("/bin/true");
	if (!data->arguments[i])
	{
		data->arguments[i] = malloc(2 * sizeof(char *));
		if (data->arguments[i])
		{
			data->arguments[i][0] = ft_strdup("/bin/true");
			data->arguments[i][1] = NULL;
		}
	}
	else if (!data->arguments[i][0])
		data->arguments[i][0] = ft_strdup("/bin/true");
}

static void	initialize_redirection_arrays(t_command_data *data)
{
	int	j;

	data->out_redirs = malloc(sizeof(t_redirection *) * data->num_commands);
	data->num_out_redirs = malloc(sizeof(int) * data->num_commands);
	if (!data->out_redirs || !data->num_out_redirs)
		return ;
	j = 0;
	while (j < data->num_commands)
	{
		data->out_redirs[j] = NULL;
		data->num_out_redirs[j] = 0;
		j++;
	}
}

static void	set_cat_arguments(t_command_data *data, int i)
{
	if (!data->arguments[i])
	{
		data->arguments[i] = malloc(2 * sizeof(char *));
		if (data->arguments[i])
		{
			data->arguments[i][0] = ft_strdup("cat");
			data->arguments[i][1] = NULL;
		}
	}
	else if (!data->arguments[i][0])
		data->arguments[i][0] = ft_strdup("cat");
}

static void	set_default_cat_command(t_command_data *data, int i)
{
	data->commands[i] = ft_strdup("cat");
	set_cat_arguments(data, i);
	if (!data->out_redirs || !data->out_redirs[i])
	{
		if (!data->out_redirs || !data->num_out_redirs)
			initialize_redirection_arrays(data);
		if (data->out_redirs && data->num_out_redirs)
		{
			data->out_redirs[i] = malloc(sizeof(t_redirection));
			if (data->out_redirs[i])
			{
				data->out_redirs[i]->file = ft_strdup("/dev/null");
				data->out_redirs[i]->append = 0;
				data->num_out_redirs[i] = 1;
			}
		}
	}
}

void	prepare_command_data_for_execution(t_command_data *data)
{
	int	i;

	if (!data)
		return ;
	i = 0;
	while (i < data->num_commands)
	{
		if (!data->commands[i] || !data->commands[i][0])
		{
			if (data->commands[i])
			{
				free(data->commands[i]);
				data->commands[i] = NULL;
			}
			if (data->heredoc_fds && data->heredoc_fds[i] != -1)
				set_default_cat_command(data, i);
			else
				set_default_true_command(data, i);
		}
		i++;
	}
}
