/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:06:32 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/11 00:32:30 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/// @brief Handles cd command errors and cleanup
/// @param shell Global shell state for exit status setting
/// @param old_pwd Previous working directory path to free
/// @return Always returns 1 (error status)
static int	handle_cd_error(t_shell *shell, char *old_pwd)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(old_pwd, 2);
	ft_putstr_fd(": No such file or directory\n", 2);
	shell->exit_status = 1;
	free(old_pwd);
	return (1);
}

static int	handle_cd_too_many_arguments(t_shell *shell)
{
	print_error_simple("cd: too many arguments", 1, shell);
	return (1);
}

/// @brief Implementation of cd builtin command with HOME and OLDPWD support
/// @param args Command arguments (cd [directory])
/// @param i Pointer to command index (unused)
/// @param shell Global shell state for environment and exit status
/// @return 0 on success, 1 on error
int	ft_cd(char **args, int *i, t_shell *shell)
{
	char	*path;
	char	*old_pwd;

	if (count_args(args) > 2)
		return (handle_cd_too_many_arguments(shell));
	old_pwd = getcwd(NULL, 0);
	if (determine_cd_path(args, &path, shell) != 0)
	{
		free(old_pwd);
		return (1);
	}
	if (chdir(path) != 0)
		return (handle_cd_error(shell, old_pwd));
	if (update_pwd_vars(old_pwd, shell) != 0)
	{
		free(old_pwd);
		return (1);
	}
	free(old_pwd);
	if (i)
		(*i)++;
	shell->exit_status = 0;
	return (1);
}
