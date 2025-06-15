/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 10:59:01 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/04 06:39:43 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	add_env_var(t_shell *shell, char *new_entry, char *name)
{
	int	len;
	int	index;

	len = 0;
	while (shell->envp[len])
		len++;
	index = find_env_var_index(shell, name);
	shell->envp = copy_envp_with_update(shell, new_entry, index);
}

int	find_env_var_index(t_shell *shell, const char *name)
{
	int	i;
	int	len;

	if (!shell->envp || !name)
		return (-1);
	len = ft_strlen(name);
	i = 0;
	while (shell->envp[i])
	{
		if (ft_strncmp(shell->envp[i], name, len) == 0 && \
				shell->envp[i][len] == '=')
			return (i);
		i++;
	}
	return (-1);
}

void	fill_updated_envp(t_shell *shell, char **new_envp, char *new_entry,
			int replace_index)
{
	int	i;
	int	j;

	if (!shell || !new_envp || !new_entry)
		return ;
	i = 0;
	j = 0;
	while (shell->envp && shell->envp[i])
	{
		if (i == replace_index)
			new_envp[j++] = ft_strdup(new_entry);
		else
			new_envp[j++] = ft_strdup(shell->envp[i]);
		free(shell->envp[i]);
		i++;
	}
	if (replace_index == -1)
		new_envp[j++] = ft_strdup(new_entry);
	new_envp[j] = NULL;
}

char	**copy_envp_with_update(t_shell *shell, char *new_entry,
			int replace_index)
{
	int		count;
	int		new_size;
	char	**new_envp;

	count = 0;
	while (shell->envp && shell->envp[count])
		count++;
	if (replace_index == -1)
		new_size = count + 2;
	else
		new_size = count + 1;
	new_envp = malloc(sizeof(char *) * new_size);
	if (!new_envp)
		return (NULL);
	fill_updated_envp(shell, new_envp, new_entry, replace_index);
	free(shell->envp);
	return (new_envp);
}

char	*create_env_entry(char *name, char *value)
{
	char	*tmp;
	char	*new_entry;

	if (!name || !value)
		return (NULL);
	tmp = ft_strjoin(name, "=");
	if (!tmp)
		return (NULL);
	new_entry = ft_strjoin(tmp, value);
	free(tmp);
	return (new_entry);
}
