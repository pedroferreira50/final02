/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 11:08:42 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/11 17:01:45 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_data_commands(char **commands, int num_commands)
{
	int	i;

	if (commands)
	{
		i = 0;
		while (i < num_commands)
		{
			if (commands[i])
				free(commands[i]);
			i++;
		}
		free(commands);
	}
}

void	free_data_arguments(char ***arguments, int num_commands)
{
	int	i;
	int	j;

	if (!arguments)
		return ;
	i = 0;
	while (i < num_commands)
	{
		if (arguments[i])
		{
			j = 0;
			while (arguments[i][j])
			{
				free(arguments[i][j]);
				j++;
			}
			free(arguments[i]);
		}
		i++;
	}
	free(arguments);
}

void	free_args(char **args, t_command_data *data)
{
	int	i;

	i = 0;
	while (args && args[i])
	{
		free(args[i]);
		i++;
	}
	if (args)
		free(args);
	if (data)
		free_command_data(data);
}

void	cleanup_command_data(t_command_data *data)
{
	if (data)
	{
		free_command_data(data);
		free(data);
	}
}

void	free_input_files(t_command_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_commands)
	{
		if (data->input_files[i])
			free(data->input_files[i]);
		i++;
	}
	free(data->input_files);
	data->input_files = NULL;
}
