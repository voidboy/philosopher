#ifndef PHILO_H
# define PHILO_H

# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <limits.h>
# include <sys/time.h>
# include <pthread.h>

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
	pthread_mutex_t	screen;
	t_bool			stop_simulation;
	int				number_of_philosophers;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				must_eat;
}				t_opts;

typedef struct s_philo {
	int				id;
	t_opts			*options;
	pthread_mutex_t	*forkR;
	pthread_mutex_t	*forkL;
	long			numb_meal;
	struct timeval	last_meal;
	struct timeval	start;
}				t_philo;

#endif
