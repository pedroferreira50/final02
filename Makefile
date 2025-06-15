NAME = minishell
CC = gcc
CFLAGS = -g -Wall -Werror -Wextra -I./includes
LIBS = -Llibft -lft -lreadline
RM = rm -rf
OBJDIR = objs

SRCS =  ./builtins/check_execute_builtins.c \
		./builtins/builtins_utils.c \
		./builtins/builtins_utils2.c  \
		./builtins/echo.c \
		./builtins/pwd.c \
		./builtins/cd.c \
		./builtins/cd_utils.c \
		./builtins/export.c \
		./builtins/export_utils.c \
		./builtins/print_env.c \
		./builtins/unset.c \
		./builtins/env.c \
		./builtins/exit.c \
		./expand_vars/calc_size.c \
		./expand_vars/expand_utils.c \
		./expand_vars/fill_expanded.c \
		./expand_vars/fill_expanded_utils.c \
		./expand_vars/var_expansion.c \
		./expand_vars/mixed_quotes.c \
		./expand_vars/parse_quote_segments.c \
		./expand_vars/token_expansion.c \
		./expand_vars/token_expansion_util.c \
		./expand_vars/token_array_processing.c \
		./expand_vars/token_processing_utils.c \
		./expand_vars/word_splitting.c \
		./parse_command/parse_command_utils.c \
		./parse_command/parse_command.c \
		./parse_command/process_token.c \
		./parse_command/validate_command.c \
		./pipes_and_execution/execute_commands.c \
		./pipes_and_execution/execute_command_pipeline.c \
		./pipes_and_execution/heredoc.c \
		./pipes_and_execution/pipeline.c \
		./pipes_and_execution/execute_utils.c \
		./pipes_and_execution/execute_utils2.c \
		./pipes_and_execution/prepare_execution.c \
		./parse_input/parse_input_utils.c ./parse_input/count_commands.c \
		./parse_input/count_commands_utils.c ./parse_input/parse_input.c \
		./parse_input/init_commands.c \
		error.c free_handling.c free_utils.c \
		handle_operator.c handle_redirect.c handle_redirect_utils.c \
		handle_heredoc.c handle_heredoc_utils.c handle_command_utils.c loc_vars.c main.c \
		expand_vars/command_processor.c main_utils.c \
		parsing_utils.c signal.c find_command_path.c remove_quotes.c \
		has_mixed_quotes.c

OBJS = $(SRCS:%.c=$(OBJDIR)/%.o)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	@$(MAKE) --no-print-directory -C libft
	@printf "\033[1;93m[LIBFT]\033[0m"
	@printf "\033[1;92m  Success!\033[0m\n"
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LIBS) -lreadline
	@printf "\033[1;93m[Minishell]\033[0m"
	@printf "\033[1;92m  Success!\033[0m\n"

clean:
	@$(MAKE) clean --no-print-directory -C libft
	@$(RM) $(OBJDIR)
	@printf "\033[1;93m[.o]\033[0m"
	@printf "\033[1;92m  Cleaned!\033[0m\n"

fclean: clean
	@$(MAKE) fclean --no-print-directory -C libft
	@$(RM) $(NAME)
	@printf "\033[1;93m[.a]\033[0m"
	@printf "\033[1;92m  Cleaned!\033[0m\n"

re: fclean all
