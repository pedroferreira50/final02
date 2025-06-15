/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 06:43:00 by pviegas-          #+#    #+#             */
/*   Updated: 2025/06/04 06:43:00 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	print_env_variable(char *env_var)
{
	char	*equal;
	char	*var_name;

	equal = ft_strchr(env_var, '=');
	if (!equal)
		return (0);
	var_name = ft_substr(env_var, 0, equal - env_var);
	if (!var_name)
		return (1);
	if (strcmp(var_name, "_") != 0)
	{
		*equal = '\0';
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		ft_putstr_fd(env_var, STDOUT_FILENO);
		ft_putstr_fd("=\"", STDOUT_FILENO);
		ft_putstr_fd(equal + 1, STDOUT_FILENO);
		ft_putstr_fd("\"\n", STDOUT_FILENO);
		*equal = '=';
	}
	free(var_name);
	return (0);
}

static int	print_environment_variables(t_shell *shell)
{
	int	i;

	i = 0;
	while (shell->envp && shell->envp[i])
	{
		if (print_env_variable(shell->envp[i]) != 0)
			return (1);
		i++;
	}
	return (0);
}

static void	print_exported_local_variable(t_var *var)
{
	ft_putstr_fd("declare -x ", STDOUT_FILENO);
	ft_putstr_fd(var->name, STDOUT_FILENO);
	ft_putstr_fd("\n", STDOUT_FILENO);
}

static void	print_exported_local_variables(t_shell *shell)
{
	t_var	*var;

	var = shell->vars;
	while (var)
	{
		if (var->exported && find_env_var_index(shell, var->name) == -1)
			print_exported_local_variable(var);
		var = var->next;
	}
}

int	print_exported_env(t_shell *shell)
{
	if (print_environment_variables(shell) != 0)
		return (1);
	print_exported_local_variables(shell);
	return (0);
}
