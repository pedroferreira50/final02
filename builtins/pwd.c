/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:08:06 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/11 04:56:21 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/// @brief Implementation of pwd builtin command -
/// prints current working directory
/// @return 0 on success, 1 on getcwd failure
int	ft_pwd(void)
{
	char	cwd[4096];

	if (getcwd(cwd, 4096))
	{
		ft_putendl_fd(cwd, 1);
		return (1);
	}
	else
		return (0);
}
