/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   count_commands_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 11:57:46 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/11 04:54:02 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/// @brief Validates pipe syntax and increments command count
/// during analysis phase
/// @param ctx Count context with args, data, and shell state
/// @param idx Current parsing position indices
void	handle_pipe_validation(t_count_context *ctx, t_indices *idx)
{
	if (idx->j == 0)
	{
		if (idx->i == 0)
		{
			print_error_simple("syntax error: no command before pipe", 2,
				ctx->shell);
			return ;
		}
	}
	if (idx->i + 1 >= (size_t)ctx->count || ctx->args[idx->i + 1] == NULL)
	{
		print_error_simple("syntax error near token", 2, ctx->shell);
		return ;
	}
	if (ft_strcmp(ctx->args[idx->i + 1], "|") == 0
		|| ft_strcmp(ctx->args[idx->i + 1], "||") == 0)
	{
		print_error_simple("syntax error near unexpected token `|'",
			2, ctx->shell);
		idx->i += 1;
		return ;
	}
}

/// @brief Processes heredoc operator (<<) during command counting phase
/// @param ctx Count context with args, data, and shell state
/// @param idx Current parsing position indices (modified to skip delimiter)
void	process_heredoc_operator(t_count_context *ctx, t_indices *idx)
{
	if (idx->i + 1 >= (size_t)ctx->count || ctx->args[idx->i + 1] == NULL)
	{
		print_error_simple("syntax error near token", 2, ctx->shell);
		idx->i += 1;
		return ;
	}
	idx->i += 2;
}

/// @brief Processes redirection operators (<, >, >>) during command counting
/// @param ctx Count context with args, data, and shell state
/// @param idx Current parsing position indices (modified to skip filename)
/// @param command_index Current command index in pipeline
void	process_redirect_operator(t_count_context *ctx, t_indices *idx,
	int command_index)
{
	t_redirect_context	redirect_ctx;

	if (idx->i + 1 >= (size_t)ctx->count || ctx->args[idx->i + 1] == NULL)
	{
		print_error_simple(" syntax error near token", 2, ctx->shell);
		idx->i += 1;
		return ;
	}
	ctx->shell->is_counting = 1;
	redirect_ctx.args = ctx->args;
	redirect_ctx.data = ctx->data;
	redirect_ctx.indices = idx;
	redirect_ctx.shell = ctx->shell;
	redirect_ctx.cmd_index = command_index;
	handle_redirect(&redirect_ctx);
}

/// @brief Validates counting context and initializes parsing state
/// @param ctx Count context to validate
/// @param idx Indices structure to initialize
/// @param command_index Command index pointer to initialize
/// @return 1 on success, 0 on invalid input
int	validate_args_and_init(t_count_context *ctx, t_indices *idx,
	int *command_index)
{
	idx->i = 0;
	idx->j = 0;
	*command_index = 0;
	ctx->data->num_pipes = 0;
	if (ctx->args == NULL)
	{
		ctx->shell->exit_status = 2;
		return (0);
	}
	return (1);
}

/// @brief Validates current argument during counting iteration
/// @param ctx Count context with args and shell state
/// @param i Current argument index to check
/// @return 1 if valid, 0 if NULL or error
int	check_current_arg(t_count_context *ctx, size_t i)
{
	if (ctx->args[i] == NULL)
	{
		ctx->shell->exit_status = 2;
		return (0);
	}
	return (1);
}
