/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_command_path.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:45:50 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/11 04:58:06 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*get_path_from_env(t_shell *shell)
{
	int	i;

	i = 0;
	while (shell->envp && shell->envp[i])
	{
		if (ft_strncmp(shell->envp[i], "PATH=", 5) == 0)
			return (shell->envp[i] + 5);
		i = i + 1;
	}
	return (NULL);
}

static char	*check_direct_executable(char *command)
{
	char		*result;
	struct stat	sb;

	result = NULL;
	if (access(command, X_OK) == 0)
	{
		if (stat(command, &sb) == 0 && !S_ISDIR(sb.st_mode))
			result = ft_strdup(command);
	}
	return (result);
}

static int	is_executable_file(char *path)
{
	struct stat	sb;

	if (access(path, X_OK) != 0)
		return (0);
	if (stat(path, &sb) != 0)
		return (0);
	if (S_ISDIR(sb.st_mode))
		return (0);
	return (1);
}

static char	*search_in_path(char *command, char **path_dirs, t_shell *shell)
{
	int		i;
	char	*temp;
	char	*full_path;

	i = 0;
	while (path_dirs[i])
	{
		temp = ft_strjoin(path_dirs[i++], "/");
		if (temp == NULL)
		{
			print_error_simple("malloc failed", 1, shell);
			return (NULL);
		}
		full_path = ft_strjoin(temp, command);
		free(temp);
		if (full_path == NULL)
		{
			print_error_simple("malloc failed", 1, shell);
			return (NULL);
		}
		if (is_executable_file(full_path))
			return (full_path);
		free(full_path);
	}
	return (NULL);
}

/// @brief Searches for executable command in PATH directories
/// or validates absolute path
/// @param command Command name or path to search for
/// @param shell Global shell state for PATH environment variable access
/// @return Full path to executable, or NULL if not found/not executable
char	*find_command_path(char *command, t_shell *shell)
{
	char	*path;
	char	**path_dirs;
	char	*result;

	if (!command || !*command)
		return (NULL);
	if (ft_strchr(command, '/'))
		return (check_direct_executable(command));
	path = get_path_from_env(shell);
	if (path != NULL)
	{
		path_dirs = ft_split(path, ':');
		if (path_dirs == NULL)
		{
			ft_putstr_fd("Error: ft_split failed\n", STDERR_FILENO);
			shell->exit_status = 1;
			return (NULL);
		}
		result = search_in_path(command, path_dirs, shell);
		free_args(path_dirs, NULL);
		if (result != NULL)
			return (result);
	}
	return (check_direct_executable(command));
}
