/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_processor.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 11:42:54 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/10 20:48:50 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*expand_tilde(char *token, t_shell *shell)
{
	char	*home;
	char	*result;
	size_t	home_len;
	size_t	token_len;

	if (!token || token[0] != '~')
		return (ft_strdup(token));
	if (token[1] != '\0' && token[1] != '/')
		return (ft_strdup(token));
	home = get_var_value_helper("HOME", shell->vars, shell->envp);
	if (!home || !*home)
		return (ft_strdup(token));
	home_len = ft_strlen(home);
	token_len = ft_strlen(token);
	result = malloc(home_len + token_len);
	if (!result)
		return (ft_strdup(token));
	ft_strcpy(result, home);
	if (token[1] == '/')
		ft_strcat(result, &token[1]);
	return (result);
}
