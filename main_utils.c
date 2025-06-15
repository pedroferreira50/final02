/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 12:25:17 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/14 01:53:12 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	set_default_path(t_shell *shell)
{
	char	*default_path;

	default_path = ft_strdup("/bin");
	if (!default_path)
		return ;
	export_var("PATH", default_path, shell);
	free(default_path);
}

static void	setup_shlvl_variable(t_shell *shell)
{
	char	*shlvl_str;
	char	*new_shlvl;
	int		shlvl;

	shlvl_str = get_env_value(shell->envp, "SHLVL");
	if (shlvl_str)
		shlvl = ft_atoi(shlvl_str) + 1;
	else
		shlvl = 1;
	new_shlvl = ft_itoa(shlvl);
	if (new_shlvl)
	{
		export_var("SHLVL", new_shlvl, shell);
		free(new_shlvl);
	}
}

void	init_shell(t_shell *shell, char **envp)
{
	char	*path;

	shell->envp = copy_envp(envp);
	shell->vars = NULL;
	shell->exit_status = 0;
	g_signal = 0;
	path = get_env_value(shell->envp, "PATH");
	if (!path)
		set_default_path(shell);
	setup_shlvl_variable(shell);
	set_signals_interactive();
}

void	finalize_shell(t_shell *shell)
{
	free_args(shell->envp, NULL);
	free_all_vars(&shell->vars);
	clear_history();
}

char	**cleanup_parse_data(t_parse_result *parsed, char **expanded_args,
		char **filtered_args)
{
	if (parsed->args)
		free_args(parsed->args, NULL);
	if (expanded_args)
		free_args(expanded_args, NULL);
	if (filtered_args)
		free_args(filtered_args, NULL);
	if (parsed->quote_types)
		free(parsed->quote_types);
	if (parsed->operator_flags)
		free(parsed->operator_flags);
	return (NULL);
}
