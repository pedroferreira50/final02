/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 18:31:59 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/14 02:01:01 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <limits.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <string.h>
# include <stdbool.h>
# include <stdio.h>
# include <signal.h>
# include <termios.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "libft/libft.h"

extern int	g_signal;

/// @brief Local shell variable node for minishell's variable management system
/// @param name Variable name (e.g., "PATH", "USER")
/// @param value Variable value string (e.g., "/bin:/usr/bin", "pedro")
/// @param exported Flag: 1 if exported to environment, 0 if local only
/// @param next Pointer to next variable in linked list (NULL if last)
typedef struct s_var
{
	char			*name;
	char			*value;
	int				exported;
	struct s_var	*next;
}	t_var;

/// @brief Intermediate result from command parsing before expansion
/// @param args Tokenized arguments array from input string
/// @param quote_types Quote context per token: '\0'=none,
/// '\''=single, '"'=double, 'M'=mixed
/// @param operator_flags Marks operator tokens ('|', '<', '>',
/// '<<', '>>') for parsing
typedef struct s_parse_result
{
	char	**args;
	char	*quote_types;
	char	*operator_flags;
}	t_parse_result;

/// @brief Output redirection specification for a single file
/// @param file Target filename for redirection
/// @param append Mode flag: 1 for append (>>), 0 for truncate (>)
typedef struct s_redirection
{
	char	*file;
	int		append;
}	t_redirection;

/// @brief Complete pipeline structure representing parsed command chain
/// @param commands Array of command names for each pipeline stage
/// @param arguments 2D array of arguments for
/// each command [cmd_idx][arg_idx]
/// @param heredoc_delims Array of heredoc delimiters
/// per command (NULL if none)
/// @param input_files Array of input file redirections
/// per command (NULL if none)
/// @param out_redirs Array of output redirection arrays per command
/// @param num_out_redirs Count of output redirections per command
/// @param num_commands Total number of commands in pipeline
/// @param num_pipes Number of pipes (num_commands - 1)
/// @param heredoc_quoted Array indicating if heredoc delimiters were quoted
/// @param heredoc_fds Array of pipe file descriptors for heredoc content
typedef struct s_command_data
{
	char			**commands;
	char			***arguments;
	char			**heredoc_delims;
	char			**input_files;
	t_redirection	**out_redirs;
	int				*num_out_redirs;
	int				num_commands;
	int				num_pipes;
	int				*heredoc_quoted;
	int				*heredoc_fds;
}	t_command_data;

/// @brief Global minishell state and environment management
/// @param envp Environment variables array (main environment)
/// @param vars Local variables linked list (shell-specific variables)
/// @param exit_status Last command exit code (for $? expansion)
/// @param is_counting Internal parser state flag
/// during command counting phase
/// @param is_save_to_execute Controls builtin execution
/// context (parent vs child)
typedef struct s_shell
{
	char	**envp;
	t_var	*vars;
	int		exit_status;
	int		is_counting;
	bool	is_save_to_execute;
}	t_shell;

/// @brief Parser state machine for tokenizing command input
/// @param cmd Source command string being parsed
/// @param args Dynamic array of parsed arguments
/// @param quote_char Current quote character context ('"', '\'', or '\0')
/// @param quote_types Quote type array parallel to args
/// @param operator_flags Operator marking array parallel to args
/// @param args_count Current number of parsed arguments
/// @param in_quotes Flag indicating if currently inside quotes
/// @param brace_count Brace nesting level (unused in current implementation)
/// @param i Current parsing position in cmd string
/// @param start Start position of current token being parsed
typedef struct s_parse
{
	char		*cmd;
	char		**args;
	char		quote_char;
	char		*quote_types;
	char		*operator_flags;
	int			args_count;
	int			in_quotes;
	int			brace_count;
	size_t		i;
	size_t		start;
}	t_parse;

/// @brief Pipeline execution state tracking during command execution
/// @param pipefd Current pipe file descriptors [read_end, write_end]
/// @param prev_pipe_read Read end of previous pipe in chain
/// @param i Current command index being executed in pipeline
typedef struct s_exec_state
{
	int	pipefd[2];
	int	prev_pipe_read;
	int	i;
}	t_exec_state;

/// @brief Generic index pair for traversing 2D structures
/// @param i Primary index (typically outer loop/array index)
/// @param j Secondary index (typically inner loop/element index)
typedef struct s_indices
{
	size_t	i;
	size_t	j;
}	t_indices;

/// @brief Parser state for command population phase
/// @param idx Index tracking for argument traversal
/// @param command_index Current command being populated in pipeline
typedef struct s_parse_state
{
	t_indices	idx;
	int			command_index;
}	t_parse_state;

/// @brief Context for command counting phase during pipeline analysis
/// @param args Input token array from parsing
/// @param count Total number of tokens in args array
/// @param data Command data structure being populated
/// @param arg_counts Array tracking argument count per command
/// @param shell Global shell state for error handling
typedef struct s_count_context
{
	char			**args;
	int				count;
	t_command_data	*data;
	int				*arg_counts;
	t_shell			*shell;
}	t_count_context;

/// @brief Parsed segment from mixed quote string analysis
/// @param content Segment text content
/// @param quote_type Quote context: '\0'=unquoted, '"'=double, '\''=single
/// @param start Start position in original string
/// @param end End position in original string
typedef struct s_quote_segment
{
	char	*content;
	char	quote_type;
	int		start;
	int		end;
}	t_quote_segment;

/// @brief Parameters for token expansion processing during word splitting
/// @param expanded Output array being built with expanded tokens
/// @param expanded_token Current token after variable expansion
/// @param i Input token index being processed
/// @param out_idx Output array index for next insertion
typedef struct s_token_process_params
{
	char	**expanded;
	char	*expanded_token;
	int		i;
	int		out_idx;
}	t_token_process_params;

/// @brief Context for processing redirection operators during parsing
/// @param args Token array being processed
/// @param data Command data structure being populated
/// @param indices Current parsing position indices
/// @param shell Global shell state for error handling
/// @param cmd_index Current command index in pipeline
typedef struct s_redirect_context
{
	char			**args;
	t_command_data	*data;
	t_indices		*indices;
	t_shell			*shell;
	int				cmd_index;
}	t_redirect_context;

/// @brief Base parameters for redirection processing functions
/// @param args Token array being processed
/// @param data Command data structure being populated
/// @param indices Current parsing position indices
/// @param cmd_index Current command index in pipeline
typedef struct s_redirect_params
{
	char			**args;
	t_command_data	*data;
	t_indices		*indices;
	int				cmd_index;
}	t_redirect_params;

/// @brief Extended parameters for output redirection processing
/// @param base Base redirection parameters (args, data, indices, cmd_index)
/// @param token Specific redirection operator token (">" or ">>")
typedef struct s_output_params
{
	t_redirect_params	base;
	const char			*token;
}	t_output_params;

/// @brief Context for heredoc processing with terminal control
/// @param shell Global shell state for variable expansion and error handling
/// @param pipefd Pipe file descriptors for heredoc content
/// [read_end, write_end]
/// @param old_termios Saved terminal settings for restoring after heredoc
typedef struct s_heredoc_context
{
	t_shell			*shell;
	int				*pipefd;
	struct termios	*old_termios;
}	t_heredoc_context;

// main.c
void			init_shell(t_shell *shell, char **envp);
int				process_input(char *input, t_shell *shell);
void			finalize_shell(t_shell *shell);
void			update_env_var(char *name, char *value, t_shell *shell);
void			handle_command(char *input, t_shell *shell);

// main_utils.c
char			**cleanup_parse_data(t_parse_result *parsed,
					char **expanded_args, char **filtered_args);

// handle_command_utils.c
void			cleanup_command_data(t_command_data *data);
char			**parse_and_expand_command(char *input, t_shell *shell,
					t_parse_result *parsed);
char			**create_filtered_args_improved(char **expanded_args,
					t_parse_result *parsed);
void			execute_command_pipeline(t_command_data *data, t_shell *shell);
void			prepare_command_data_for_execution(t_command_data *data);
// signal.c
void			set_signals_interactive(void);
void			set_signals_simple(int handle_sigquit);
void			ignore_sigquit(void);
void			signal_simple_interrupt(int signal);

//handle_operator.c

void			handle_pipe(t_command_data *data, int *command_index,
					t_shell *shell);
// handle_redirect functions
int				validate_redirect_syntax(char **args, t_indices *indices,
					t_shell *shell);
int				open_output_file(char **args, t_indices *indices,
					const char *token, t_shell *shell);
int				add_output_redirection(t_output_params *params);
int				handle_output_redirect(t_output_params *params, t_shell *shell);
void			handle_redirect(t_redirect_context *ctx);
int				set_redirection_data(t_output_params *params);
int				process_input_file(t_redirect_params *params, t_shell *shell);
int				update_input_files(t_redirect_params *params, t_shell *shell);

// handle_heredoc functions
char			*process_quoted_delimiter(char *delim, size_t len);
char			*clean_mixed_quotes(char *delim, size_t len);
void			set_heredoc_delimiter(t_command_data *data,
					char *delim, int cmd_index);
int				validate_heredoc_args(char **args, t_indices *indices,
					t_shell *shell);
void			handle_heredoc(t_redirect_context	*redirect_ctx);
void			print_heredoc_eof_warning(const char *delimiter);
void			prepare_heredoc_terminal(int tty_fd,
					struct termios *old_termios);
int				process_heredoc_loop(const char *delimiter,
					int suppress_expansion, t_heredoc_context *ctx);
char			*read_heredoc_line(void);
void			write_heredoc_line(int pipefd, char *line,
					int suppress_expansion, t_shell *shell);
int				cleanup_heredoc(int *pipefd, int exit_code,
					t_shell *shell, char *line);

// env_utils.c
char			**copy_envp(char **envp);

// parsing_utils.c
int				count_args(char **args);
void			add_argument(t_parse *state);
void			alloc_commands(t_command_data *data, t_shell *shell);
char			*clean_escaped_token(const char *token);

// string_utils.c
char			*ft_strndup(const char *src, size_t n);

//error.c
int				error_operator(char op, t_shell *shell);
// Enhanced error handling utility functions
void			print_error_simple(const char *message, int exit_code,
					t_shell *shell);
int				print_error_command(const char *command, const char *message);
void			print_error_token(const char *token, int exit_code,
					t_shell *shell);
void			print_error_command2(const char *command, const char *file,
					const char *message);

void			initialize_state(t_parse *state, char *cmd);
int				check_errors(t_parse *state, t_shell *shell,
					int last_was_operator);
void			finalize_result(t_parse *state, t_parse_result *result,
					const char *cmd);
// process_token.c
void			process_token(t_parse *state, int *last_was_operator);
//parse_input_utils.c
int				is_operator(const char *token);
int				*initialize_arg_counts(int count, t_shell *shell);
void			finalize_arguments(t_command_data *data, int *arg_counts);
//parse_input.c
void			parse_input(char **args, int count, t_command_data *data,
					t_shell *shell);
char			*process_filename_token(const char *token);
void			populate_command(char **args, int *arg_counts,
					t_command_data *data, t_parse_state *state);
void			populate_argument(char **args, t_command_data *data,
					t_parse_state *state);
void			populate_commands(char **args, int *arg_counts,
					t_command_data *data, t_shell *shell);

//parse_input/count_commands.c
void			count_commands(t_count_context	*ctx);

//parse_input/count_commands_utils.c
void			handle_pipe_validation(t_count_context *ctx, t_indices *idx);
void			process_heredoc_operator(t_count_context *ctx, t_indices *idx);
void			process_redirect_operator(t_count_context *ctx, t_indices *idx,
					int command_index);
int				validate_args_and_init(t_count_context *ctx, t_indices *idx,
					int *command_index);
int				check_current_arg(t_count_context *ctx, size_t i);

//builtins
// check_execute_builtins.c
int				child_builtin(int *i, t_shell *shell, t_command_data *data);
int				parent_builtin(t_command_data *data, t_exec_state *state,
					t_shell *shell);

// builtins_utils.c
int				ft_isspace(int c);
int				check_builtin(char *command);
int				execute_builtin_command(t_command_data *data,
					char **args, t_shell *shell, int *i);
int				handle_input_redirection(t_command_data *data, int *i,
					int original_stdin, t_shell *shell);
int				handle_output_redirection(t_command_data *data, int *i,
					int original_stdout, t_shell *shell);
void			restore_fds(int original_stdin, int original_stdout);
//cd
int				ft_cd(char **args, int *i, t_shell *shell);
//cd_utils
char			*get_env_value(char **envp, const char *name);
int				determine_cd_path(char **args, char **path, t_shell *shell);
int				update_pwd_vars(char *old_pwd, t_shell *shell);
int				gotohome(char **path, t_shell *shell);
int				goto_oldpwd(char **path, t_shell *shell);
//echo
void			ft_echo(char **args, t_shell *shell);
//env
void			ft_env(char **envp, t_shell *shell);
//export
int				ft_export(char **args, t_shell *shell);
void			export_var(char *name, char *value, t_shell *shell);
int				print_exported_env(t_shell *shell);
int				find_env_var_index(t_shell *shell, const char *name);
char			**copy_envp_with_update(t_shell *shell, char *new_entry,
					int replace_index);
void			add_env_var(t_shell *shell, char *new_entry, char *name);
char			*create_env_entry(char *name, char *value);
void			insertion_sort_env(char **env);
int				handle_invalid_identifier(char *name, t_shell *shell);
int				handle_builtin_invalid_option(char *arg,
					char *cmd_name, int is_env);
//pwd
int				ft_pwd(void);
//unset
int				ft_unset(char **args, t_var **vars, char ***envp);
//exit
void			ft_exit(char **args, t_shell *shell, t_command_data *data);
//loc_vars
int				is_valid_var_name(const char *name);
void			store_var(char *name, char *value, t_var **vars);

int				is_var_assignment(const char *input);
void			handle_var_assignment(const char *input, t_shell *shell);

//expand_vars folder
// calc_size.c
size_t			calc_expanded_size(const char *arg, char quote_type,
					t_shell *shell);
// fill_expand.c
void			fill_expanded(char *dest, const char *src, char quote_type,
					t_shell *shell);
// fill_expanded_utils.c
void			fill_quotes(char *dest, const char *src, t_indices *indices,
					char quote_type);
void			fill_exit_status(char *dest, t_indices *indices,
					t_shell *shell);
void			fill_var_name(char *dest, const char *src, t_indices *indices,
					t_shell *shell);
// var_expansion.c
size_t			calc_var_size(const char *arg, t_indices *indices,
					t_shell *shell);
// var_expansion.c
size_t			handle_exit_status(char *dest, int fill, size_t *i,
					t_shell *shell);
size_t			get_var_len(const char *str, size_t i, t_var *vars,
					char **envp);
int				var_exists(const char *name, t_var *vars, char **envp);
char			*get_var_value_helper(const char *name, t_var *vars,
					char **envp);
char			*expand_variables(const char *arg, char quote_type,
					t_shell *shell);
char			*expand_mixed_quotes(const char *str, t_shell *shell);

// parse_quote_segments.c
int				count_segments(const char *str);
int				find_quote_end(const char *str, int start, char quote_char);
t_quote_segment	*parse_quote_segments(const char *str, int *count);

//find_command_path.c
char			*find_command_path(char *command, t_shell *shell);

//pipes_and_execution folder
// pipeline.c
void			finalize_execution(t_exec_state *state, pid_t *pids,
					t_command_data *data, t_shell *shell);
void			setup_pipes_and_redirections(t_command_data *data,
					t_exec_state *state, int num_commands, t_shell *shell);
int				setup_pipeline(t_command_data *data, t_exec_state *state,
					t_shell *shell);
// heredoc.c
int				setup_heredoc(const char *delimiter, t_shell *shell,
					int suppress_expansion);

//  execute_commands.c
void			run_pipeline(t_command_data *data, t_exec_state *state,
					t_shell *shell, pid_t *pids);
void			fork_child(t_command_data *data, t_exec_state *state,
					t_shell *shell, pid_t *pids);
void			execute_command(int *i, t_shell *shell, pid_t *pids,
					t_command_data *data);
void			handle_wait_status(int status, t_shell *shell);
void			execute_commands(t_command_data *data, t_shell *shell);
void			wait_commands(pid_t *pids, t_command_data *data,
					t_shell *shell);
//expand_vars/command_processor.c
void			expand_and_validate(char **tokens, char *quote_types,
					t_shell *shell);
t_parse_result	parse_command(char *cmd, t_shell *shell);

char			**expand_tokens(char **tokens, char *quote_types,
					t_shell *shell);
char			*expand_tilde(char *token, t_shell *shell);
int				validate_command(char **args, t_shell *shell);
int				has_mixed_quotes(const char *str);

//token_expansion.c
char			*expand_single_token(char *token, char quote_type,
					t_shell *shell, int is_heredoc);
int				is_heredoc_delimiter(char **tokens, char *quote_types,
					int index);
char			*protect_operators_and_expand_tilde(char *expanded, char *token,
					char quote_type, t_shell *shell);
char			*quote_wrap_operator(char *str);
char			*encode_operators_in_string(char *str);
int				is_operator_small(char *expanded, char *token, char quote_type);
int				contains_operators_from_variable(char *expanded,
					char *token, char quote_type);

//token_processing_utils.c
void			process_single_expanded_token(char **tokens, char *quote_types,
					t_shell *shell, t_token_process_params *params);
int				should_split_this_token(char **tokens, char *quote_types, int i,
					char *expanded_token);
void			add_split_tokens(char ***expanded, int *out_idx,
					char *expanded_token);

//word_splitting.c
char			**split_expanded_token(const char *expanded_token);
int				should_split_token(char quote_type, const char *expanded_token);

//remove_quotes.c
char			*remove_quotes(char *str);

//free
void			free_data_commands(char **commands, int num_commands);
void			free_data_arguments(char ***arguments, int num_commands);
void			free_all_vars(t_var **vars);
void			free_args(char **args, t_command_data *data);
void			free_command_data(t_command_data *data);
void			free_state(t_parse *state);
void			free_state_args(t_parse *state);
void			free_input_files(t_command_data *data);

#endif
