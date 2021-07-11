#ifndef PHILO_H
# define PHILO_H

# include <stdlib.h>
# include <stdio.h>
# include <signal.h>
# include <unistd.h>
# include <limits.h>
# include <sys/time.h>
# include <pthread.h>
# include <semaphore.h>

typedef enum e_states {
	FORK,
	EAT,
	SLEEP,
	THINK,
}			t_states;

typedef enum e_bool {
	FALSE,
	TRUE,
}			t_bool;

typedef struct s_opts {
	sem_t			*screen;
	t_bool			stop_simulation;
	int				number_of_philosophers;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				must_eat;
}				t_opts;

typedef struct s_philo {
	int				id;
	pid_t			pid;
	t_opts			options;
	sem_t			*forks;
	long			numb_meal;
	struct timeval	last_meal;
	struct timeval	start;
}				t_philo;

#endif
