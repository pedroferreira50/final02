/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:07:00 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/09 16:07:00 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/// @brief Parses and skips -n flags at start of echo arguments
/// @param args Echo command arguments array
/// @param newline Pointer to newline flag (set to 0 if -n found)
/// @return Index of first non-flag argument
static int	skip_n_flags(char **args, int *newline)
{
	int	i;
	int	j;

	i = 1;
	*newline = 1;
	while (args[i] && ft_strncmp(args[i], "-n", 2) == 0)
	{
		j = 2;
		while (args[i][j] == 'n')
			j++;
		if (args[i][j] != '\0')
			break ;
		*newline = 0;
		i++;
	}
	return (i);
}

/// @brief Implementation of echo builtin command with -n flag support
/// @param args Command arguments including echo command name at index 0
/// @param shell Global shell state for exit status setting
void	ft_echo(char **args, t_shell *shell)
{
	int	i;
	int	newline;

	i = skip_n_flags(args, &newline);
	while (args[i])
	{
		ft_putstr_fd(args[i], STDOUT_FILENO);
		if (args[i + 1])
			ft_putchar_fd(' ', STDOUT_FILENO);
		i++;
	}
	if (newline)
		ft_putchar_fd('\n', STDOUT_FILENO);
	shell->exit_status = 0;
}
