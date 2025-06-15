/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 17:20:50 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/09 19:05:16 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	check_builtin(char *command)
{
	if (command == NULL)
		return (0);
	if (ft_strcmp(command, "echo") == 0 || \
		ft_strcmp(command, "pwd") == 0 || \
		ft_strcmp(command, "cd") == 0 || \
		ft_strcmp(command, "export") == 0 || \
		ft_strcmp(command, "unset") == 0 || \
		ft_strcmp(command, "exit") == 0 || \
		ft_strcmp(command, "env") == 0)
		return (1);
	return (0);
}

/* const char	*get_var_value(const char *name, t_var *vars)
{
	t_var	*var;

	var = vars;
	while (var)
	{
		if (ft_strcmp(var->name, name) == 0)
			return (var->value);
		var = var->next;
	}
	return (NULL);
} */

int	ft_isspace(int c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || \
		c == '\f' || c == '\r');
}

int	handle_invalid_identifier(char *name, t_shell *shell)
{
	ft_putstr_fd("minishell: export: `", 2);
	ft_putstr_fd(name, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
	shell->exit_status = 1;
	return (1);
}

void	insertion_sort_env(char **env)
{
	int		i;
	int		j;
	char	*key;

	i = 1;
	while (env[i])
	{
		key = env[i];
		j = i - 1;
		while (j >= 0 && ft_strcmp(env[j], key) > 0)
		{
			env[j + 1] = env[j];
			j--;
		}
		env[j + 1] = key;
		i++;
	}
}
