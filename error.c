/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 17:23:43 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/11 16:18:28 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/// @brief Handles syntax errors for shell operators with appropriate exit codes
/// @param op Operator character causing the error ('|', '<', '>')
/// @param shell Global shell state for exit status setting
/// @return Always returns 0 (indicates parse failure)
int	error_operator(char op, t_shell *shell)
{
	if (op == '|')
		print_error_token("|", 2, shell);
	else if (op == '<')
		print_error_token("<", 2, shell);
	else
		print_error_token(">", 2, shell);
	return (0);
}

void	print_error_simple(const char *message, int exit_code, t_shell *shell)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd((char *)message, 2);
	ft_putstr_fd("\n", 2);
	shell->exit_status = exit_code;
}

int	print_error_command(const char *command, const char *message)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd((char *)command, 2);
	ft_putstr_fd(": ", 2);
	ft_putstr_fd((char *)message, 2);
	ft_putstr_fd("\n", 2);
	return (1);
}

/// @brief Prints syntax error message for unexpected tokens
/// @param token Token causing the syntax error
/// @param exit_code Exit status to set (typically 2 for syntax errors)
/// @param shell Global shell state for exit status setting
void	print_error_token(const char *token, int exit_code, t_shell *shell)
{
	ft_putstr_fd("minishell: syntax error unexpected token '", 2);
	ft_putstr_fd((char *)token, 2);
	ft_putstr_fd("'\n", 2);
	shell->exit_status = exit_code;
}

void	print_error_command2(const char *command, const char *file,
		const char *message)
{
	ft_putstr_fd((char *)command, 2);
	ft_putstr_fd(": ", 2);
	ft_putstr_fd("‘", 2);
	ft_putstr_fd((char *)file, 2);
	ft_putstr_fd("’", 2);
	ft_putstr_fd(": ", 2);
	ft_putstr_fd((char *)message, 2);
	ft_putstr_fd("\n", 2);
}
