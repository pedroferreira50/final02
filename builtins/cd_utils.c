/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 06:51:24 by pviegas-          #+#    #+#             */
/*   Updated: 2025/06/11 00:32:44 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*get_env_value(char **envp, const char *name)
{
	size_t	name_len;
	int		i;

	if (!envp || !name)
		return (NULL);
	name_len = ft_strlen(name);
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], name, name_len) == 0
			&& envp[i][name_len] == '=')
			return (envp[i] + name_len + 1);
		i++;
	}
	return (NULL);
}

int	determine_cd_path(char **args, char **path, t_shell *shell)
{
	if (args[1] == NULL || strcmp(args[1], "~") == 0)
		return (gotohome(path, shell));
	else if (strcmp(args[1], "-") == 0)
		return (goto_oldpwd(path, shell));
	else
	{
		*path = args[1];
		return (0);
	}
}

int	gotohome(char **path, t_shell *shell)
{
	*path = get_env_value(shell->envp, "HOME");
	if (*path == NULL)
	{
		print_error_simple("cd: HOME not set", 1, shell);
		return (1);
	}
	return (0);
}

int	goto_oldpwd(char **path, t_shell *shell)
{
	*path = get_env_value(shell->envp, "OLDPWD");
	if (*path == NULL)
	{
		print_error_simple("cd: OLDPWD not set", 1, shell);
		return (1);
	}
	printf("%s\n", *path);
	return (0);
}

int	update_pwd_vars(char *old_pwd, t_shell *shell)
{
	char	*new_pwd;

	new_pwd = getcwd(NULL, 0);
	if (!new_pwd)
	{
		print_error_simple("cd: getcwd failed", 1, shell);
		free(old_pwd);
		return (1);
	}
	if (old_pwd)
		export_var("OLDPWD", old_pwd, shell);
	export_var("PWD", new_pwd, shell);
	free(new_pwd);
	return (0);
}
