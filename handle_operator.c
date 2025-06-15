/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_operator.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 11:19:51 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/04 11:19:52 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_pipe(t_command_data *data, int *command_index, t_shell *shell)
{
	data->num_pipes++;
	(*command_index)++;
	shell->exit_status = 0;
}
