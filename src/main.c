/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yharwyn- <yharwyn-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/20 17:41:11 by yharwyn-          #+#    #+#             */
/*   Updated: 2019/06/24 11:47:33 by yharwyn-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void		shell_init(void)
{
	char	*username;

	username = getenv("USER");
	CLEAR();
	ft_printf("\n\n\n\n******************"
	"****************************");
	ft_printf("\n\n\n\t*******MINISHELL PROJECT*******");
	ft_printf("\n\n\t-school 21 unix branch project-");
	ft_printf("\n\t-use   at  your  own  risk  :D-");
	ft_printf("\n\n\n\n*******************"
	"***************************");
	ft_printf("\n\n\nUSER is: @%s", username);
	ft_printf("\n");
	sleep(1);
	CLEAR();
}

void sigint_handler(int signal)
{
	printf("\n");
}

void mysh_update_cwd_info() {
	getcwd(shell->cur_dir, sizeof(shell->cur_dir));
}

void mysh_init() {
	struct sigaction sigint_action = {
			.sa_handler = &sigint_handler,
			.sa_flags = 0
	};
	sigemptyset(&sigint_action.sa_mask);
	sigaction(SIGINT, &sigint_action, NULL);

	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);

	pid_t pid = getpid();
	setpgid(pid, pid);
	tcsetpgrp(0, pid);

	shell = (struct shell_info*) malloc(sizeof(struct shell_info));
	getlogin_r(shell->cur_user, sizeof(shell->cur_user));

	struct passwd *pw = getpwuid(getuid());
	strcpy(shell->pw_dir, pw->pw_dir);

	int i;
	for (i = 0; i < NR_JOBS; i++) {
		shell->jobs[i] = NULL;
	}

	mysh_update_cwd_info();
}

void mysh_print_promt() {
	printf(COLOR_CYAN "%s" COLOR_NONE " in " COLOR_YELLOW "%s" COLOR_NONE "\n", shell->cur_user, shell->cur_dir);
	printf(COLOR_CYAN "21sh>" COLOR_NONE " ");
}

void check_zombie() {
	int status, pid;
//	while ((pid = waitpid(-1, &status, WNOHANG|WUNTRACED|WCONTINUED)) > 0) {
//		if (WIFEXITED(status)) {
//			set_process_status(pid, STATUS_DONE);
//		} else if (WIFSTOPPED(status)) {
//			set_process_status(pid, STATUS_SUSPENDED);
//		} else if (WIFCONTINUED(status)) {
//			set_process_status(pid, STATUS_CONTINUED);
//		}
//
//		int job_id = get_job_id_by_pid(pid);
//		if (job_id > 0 && is_job_completed(job_id)) {
//			print_job_status(job_id);
//			remove_job(job_id);
//		}
//	}
}

static void	free_args(char **args)
{
	int i;

	i = 0;
	while (args[i] != 0)
	{
		ft_memdel((void **)&args[i]);
		i++;
	}
	ft_memdel((void**)&args);
}

void		shell_loop(void)
{
	char		*line;
	char		**args;
	struct job	*job;
	int			status;

//	shell_init();
	mysh_init();
	status = 1;
	while (status)
	{
		signal(SIGINT, signal_handler);
//		ft_printf("$> ");
		mysh_print_promt();
		line = read_ln();
		if (strlen(line) == 0) {
			check_zombie();
			continue;
		}
		args = line_split(line, SPLIT_DELIM);
		status = launch_dispatcher(args);
		free(line);
		free_args(args);
	}
}

void		env_init(t_vault *ptr)
{
	g_env = ft_memalloc(sizeof(t_env));
	g_env->c_env = ft_memalloc(300);
	g_env->index = 0;
	g_env->vault = ptr;
	g_env->update_envv = update_env;
	g_env->print_envv = traverse;
	g_env->get_dir = get_cwd;
	g_env->get_dir(search_key(g_env->vault, "PWD")->path);
	g_env->get_dir(search_key(g_env->vault, "OLDPWD")->path);
	g_env->builtin_func[0] = &cd_shell;
	g_env->builtin_func[1] = &help_shell;
	g_env->builtin_func[2] = &exit_shell;
	g_env->builtin_func[3] = &set_env;
	g_env->builtin_func[4] = &set_env;
	g_env->builtin_func[5] = &unset_env;
	g_env->builtin_func[6] = &echo;
	g_env->i = 0;
	ft_strcpy(g_env->builtin_str[0], "cd");
	ft_strcpy(g_env->builtin_str[1], "help");
	ft_strcpy(g_env->builtin_str[2], "exit");
	ft_strcpy(g_env->builtin_str[3], "env");
	ft_strcpy(g_env->builtin_str[4], "setenv");
	ft_strcpy(g_env->builtin_str[5], "unsetenv");
	ft_strcpy(g_env->builtin_str[6], "echo");
}

int			main(void)
{
	t_vault *ptr;

	ptr = NULL;
	ptr = environ_grab(ptr);
	env_init(ptr);
	traverse(g_env->vault, grab_vault);
	shell_loop();

//	clean_up();
//	dispose_env(g_env->vault);
//	dispose_env(ptr);
	return (0);
}
