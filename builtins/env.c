/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:07:07 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/11 04:55:51 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/// @brief Frees allocated environment array copy
/// @param env Environment array to free (can be NULL)
static void	free_env_copy(char **env)
{
	int	i;

	i = 0;
	while (env && env[i])
		free(env[i++]);
	free(env);
}

/// @brief Creates deep copy of environment variable array
/// @param envp Source environment array to copy
/// @return Newly allocated copy of envp array, NULL on allocation failure
char	**copy_envp(char **envp)
{
	int		i;
	char	**new_envp;

	i = 0;
	while (envp[i])
		i++;
	new_envp = malloc((i + 1) * sizeof(char *));
	if (!new_envp)
		return (NULL);
	i = 0;
	while (envp[i])
	{
		new_envp[i] = ft_strdup(envp[i]);
		if (!new_envp[i])
			return (free_env_copy(new_envp), NULL);
		i++;
	}
	new_envp[i] = NULL;
	return (new_envp);
}

/// @brief Prints all environment variables to stdout
/// @param env Environment array to print (each entry on separate line)
static void	print_env(char **env)
{
	int	i;

	i = 0;
	while (env[i])
		printf("%s\n", env[i++]);
}

/// @brief Implementation of env builtin command -
/// prints environment variables
/// @param args Command arguments (env supports no
/// options in this implementation)
/// @param shell Global shell state for environment
/// access and error handling
void	ft_env(char **args, t_shell *shell)
{
	char	**copy;

	if (args && args[1])
	{
		if (args[1][0] == '-')
		{
			shell->exit_status
				= handle_builtin_invalid_option(args[1], "env", 1);
			return ;
		}
		print_error_command2("env", args[1], "No such file or directory");
		shell->exit_status = 127;
		return ;
	}
	copy = copy_envp(shell->envp);
	if (!copy)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	print_env(copy);
	free_env_copy(copy);
	shell->exit_status = 0;
}

/// @brief Updates or creates environment variable in shell's envp array
/// @param name Variable name to update
/// @param value New value for the variable
/// @param shell Global shell state containing envp array to modify
void	update_env_var(char *name, char *value, t_shell *shell)
{
	char	*temp;
	char	*new_entry;
	int		index;

	temp = ft_strjoin(name, "=");
	new_entry = ft_strjoin(temp, value);
	free(temp);
	if (!shell->envp)
	{
		shell->envp = malloc(sizeof(char *) * 2);
		if (!shell->envp)
		{
			free(new_entry);
			return ;
		}
		shell->envp[0] = ft_strdup(new_entry);
		shell->envp[1] = NULL;
		free(new_entry);
		return ;
	}
	index = find_env_var_index(shell, name);
	shell->envp = copy_envp_with_update(shell, new_entry, index);
	free(new_entry);
}
