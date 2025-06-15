/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_command_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 03:10:00 by pviegas-          #+#    #+#             */
/*   Updated: 2025/06/04 03:10:00 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	handle_empty_command_filtered(char **filtered_args,
		t_parse_result *parsed, t_shell *shell)
{
	int	count;

	count = 0;
	while (filtered_args[count])
		count++;
	if (count == 0)
	{
		cleanup_parse_data(parsed, NULL, filtered_args);
		shell->exit_status = 0;
		return (true);
	}
	if (filtered_args[0] && !*filtered_args[0])
	{
		print_error_command("", "command not found");
		shell->exit_status = 127;
		cleanup_parse_data(parsed, NULL, filtered_args);
		return (true);
	}
	return (false);
}

char	**parse_and_expand_command(char *input, t_shell *shell,
		t_parse_result *parsed)
{
	char	**expanded_args;
	char	**filtered_args;

	*parsed = parse_command(input, shell);
	if (!parsed->args)
		return (NULL);
	expanded_args = expand_tokens(parsed->args, parsed->quote_types, shell);
	if (!expanded_args)
		return (cleanup_parse_data(parsed, NULL, NULL));
	filtered_args = create_filtered_args_improved(expanded_args, parsed);
	if (!filtered_args)
	{
		free_args(expanded_args, NULL);
		return (cleanup_parse_data(parsed, NULL, NULL));
	}
	if (handle_empty_command_filtered(filtered_args, parsed, shell))
	{
		free_args(expanded_args, NULL);
		return (NULL);
	}
	free_args(expanded_args, NULL);
	return (filtered_args);
}

static int	should_keep_empty_arg(char *original_token, char quote_type)
{
	if (quote_type == '"' || quote_type == '\'')
		return (1);
	if (original_token && ((ft_strcmp(original_token, "\"\"") == 0)
			|| (ft_strcmp(original_token, "''") == 0)))
		return (1);
	return (0);
}

static int	filter_and_copy_args(char **filtered_args, char **expanded_args,
	t_parse_result *parsed)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (expanded_args[i])
	{
		if (expanded_args[i][0] != '\0' ||
			should_keep_empty_arg(parsed->args[i], parsed->quote_types[i]))
		{
			if (filtered_args)
				filtered_args[j] = ft_strdup(expanded_args[i]);
			j++;
		}
		i++;
	}
	if (filtered_args)
		filtered_args[j] = NULL;
	return (j);
}

char	**create_filtered_args_improved(char **expanded_args,
	t_parse_result *parsed)
{
	char	**filtered_args;
	int		count;

	count = filter_and_copy_args(NULL, expanded_args, parsed);
	filtered_args = malloc(sizeof(char *) * (count + 1));
	if (!filtered_args)
		return (NULL);
	filter_and_copy_args(filtered_args, expanded_args, parsed);
	return (filtered_args);
}
