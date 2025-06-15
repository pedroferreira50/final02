/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:08:13 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/11 04:56:05 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	handle_builtin_invalid_option(char *arg, char *cmd_name, int is_env)
{
	char	invalid_char;

	if (!is_env)
		ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd_name, 2);
	if (arg[1])
		invalid_char = arg[1];
	else
		invalid_char = '?';
	if (is_env)
	{
		ft_putstr_fd(": invalid option -- '", 2);
		ft_putchar_fd(invalid_char, 2);
		ft_putstr_fd("'\n", 2);
	}
	else
	{
		ft_putstr_fd(": -", 2);
		ft_putchar_fd(invalid_char, 2);
		ft_putstr_fd(": invalid option\n", 2);
	}
	if (is_env)
		return (125);
	return (2);
}

static void	remove_from_vars(char *name, t_var **vars)
{
	t_var	*current;
	t_var	*prev;

	if (!name || !vars || !*vars)
		return ;
	current = *vars;
	prev = NULL;
	while (current)
	{
		if (current->name && ft_strcmp(current->name, name) == 0)
		{
			if (prev)
				prev->next = current->next;
			else
				*vars = current->next;
			if (current->name)
				free(current->name);
			if (current->value)
				free(current->value);
			free(current);
			return ;
		}
		prev = current;
		current = current->next;
	}
}

static void	shift_envp(char ***envp, int start_idx, int len)
{
	int	i;

	i = start_idx;
	while (i < len - 1)
	{
		(*envp)[i] = (*envp)[i + 1];
		i++;
	}
	(*envp)[len - 1] = NULL;
}

static void	remove_from_env(char *name, char ***envp)
{
	int	i;
	int	len;

	if (!name || !envp || !*envp)
		return ;
	i = 0;
	len = 0;
	while ((*envp)[len])
		len++;
	while (i < len && (*envp)[i])
	{
		if ((*envp)[i] && ft_strncmp((*envp)[i], name, ft_strlen(name)) == 0
			&& (*envp)[i][ft_strlen(name)] == '=')
		{
			free((*envp)[i]);
			shift_envp(envp, i, len);
			return ;
		}
		i++;
	}
}

/// @brief Implementation of unset builtin - removes variables
/// from environment and local vars
/// @param args Command arguments (unset name1 name2...)
/// @param vars Pointer to local variables linked list
/// @param envp Pointer to environment variables array
/// @return 0 on success, 1 on error (invalid identifier)
int	ft_unset(char **args, t_var **vars, char ***envp)
{
	int	i;

	if (!args || !vars || !envp)
		return (1);
	i = 1;
	while (args[i])
	{
		if (args[i][0] == '\0')
		{
			i++;
			continue ;
		}
		if (args[i][0] == '-' && args[i][1] != '\0')
			return (handle_builtin_invalid_option(args[i], "unset", 0));
		if (!is_valid_var_name(args[i]))
		{
			i++;
			continue ;
		}
		remove_from_vars(args[i], vars);
		remove_from_env(args[i], envp);
		i++;
	}
	return (0);
}
