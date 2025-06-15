/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:05:58 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/15 03:38:45 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	is_numeric(const char *str)
{
	int	i;

	if (!str || str[0] == '\0')
		return (0);
	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	if (str[i] == '\0')
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

static void	print_numeric_error(const char *arg,
		t_shell *shell, t_command_data *data)
{
	char	*msg;
	char	*tmp;

	tmp = ft_strjoin("minishell: exit: ", arg);
	msg = ft_strjoin(tmp, ": numeric argument required\n");
	free(tmp);
	ft_putstr_fd(msg, STDERR_FILENO);
	free(msg);
	shell->exit_status = 2;
	cleanup_command_data(data);
	finalize_shell(shell);
	exit(2);
}

void	ft_exit(char **args, t_shell *shell, t_command_data *data)
{
	int	exit_code;

	if (args[1])
	{
		if (!is_numeric(args[1]))
			print_numeric_error(args[1], shell, data);
		if (args[2])
		{
			print_error_simple("exit: too many arguments", 1, shell);
			shell->exit_status = 1;
			return ;
		}
		exit_code = ft_atoi(args[1]) % 256;
	}
	else
		exit_code = shell->exit_status;
	shell->exit_status = exit_code;
	cleanup_command_data(data);
	finalize_shell(shell);
	ft_putstr_fd("exit\n", STDOUT_FILENO);
	exit(exit_code);
}
