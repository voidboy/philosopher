#include <stdio.h>
#include "philo.h"
#include "utils.h"

long	ft_delta(struct timeval ref)
{
	struct timeval	delta;

	gettimeofday(&delta, NULL);
	return ((delta.tv_sec * 1e3 + delta.tv_usec * 1e-3)
			- (ref.tv_sec * 1e3 + ref.tv_usec * 1e-3));
}

void	ft_show_state(t_philo *p, t_states s)
{
	static const char	*states[] = {"has taken a fork", "is eating",
		"is sleeping", "is thinking", "died"};

	sem_wait(p->options.screen);
	if (!p->options.stop_simulation)
		printf("%5ld %3d %s\n", ft_delta(p->start), p->id + 1, states[s]);
	sem_post(p->options.screen);
}

int	ft_init_philo(t_opts options, t_philo *philo, sem_t *forks)
{
	struct timeval	start;
	int				i;

	if (gettimeofday(&start, NULL) == -1)
		return (-1);
	i = -1;
	while (++i < options.number_of_philosophers)
		philo[i] = (t_philo){i, 0, options, forks, 0, start, start};
	return (0);
}

int	ft_alloc(t_opts options, pthread_t **threads, t_philo **philo)
{
	*threads = malloc(sizeof(pthread_t) * options.number_of_philosophers);
	if (!*threads)
	{
		write(STDERR_FILENO, "malloc error\n", 13);
		return (-1);
	}
	*philo = malloc(sizeof(t_philo) * options.number_of_philosophers);
	if (!*philo)
	{
		free(*threads);
		write(STDERR_FILENO, "malloc error\n", 13);
		return (-1);
	}
	return (0);
}

t_bool	is_valid_options(char *argv[], t_opts *options)
{
	int	n;

	if (ft_atoi(argv[1], &n) == FALSE || n < 0)
		return (FALSE);
	options->number_of_philosophers = n;
	if (ft_atoi(argv[2], &n) == FALSE || n < 0)
		return (FALSE);
	options->time_to_die = n;
	if (ft_atoi(argv[3], &n) == FALSE || n < 0)
		return (FALSE);
	options->time_to_eat = n * 1e3;
	if (ft_atoi(argv[4], &n) == FALSE || n < 0)
		return (FALSE);
	options->time_to_sleep = n * 1e3;
	if (argv[5])
	{
		if (ft_atoi(argv[5], &n) == FALSE || n < 0)
			return (FALSE);
		options->must_eat = n;
	}
	else
		options->must_eat = -1;
	return (TRUE);
}

void	ft_philosopher(t_philo *p)
{
	if (!(!p->id || !(p->id % 2)))
		usleep(p->options.time_to_eat);
	while (!p->options.stop_simulation && (p->options.must_eat == -1
				|| p->numb_meal < p->options.must_eat))
	{
		sem_wait(p->forks);
		ft_show_state(p, FORK);
		sem_wait(p->forks);
		ft_show_state(p, FORK);
		ft_show_state(p, EAT);
		if (++p->numb_meal == p->options.must_eat)
			exit(0);
		gettimeofday(&p->last_meal, NULL);
		usleep(p->options.time_to_eat);
		sem_post(p->forks);
		sem_post(p->forks);
		ft_show_state(p, SLEEP);
		usleep(p->options.time_to_sleep);
		ft_show_state(p, THINK);
	}
	exit(0);
}

void	*ft_monitoring(void *p)
{
	t_philo *philo = (t_philo *)p;
	int 	id;
	int		jj;

	while (1)
	{
		if (ft_delta(philo->last_meal) >= philo->options.time_to_die)
		{
			sem_wait(philo->options.screen);
			philo->options.stop_simulation = TRUE;
			if (!(philo->numb_meal == philo->options.must_eat))
			{
				printf("%5ld %3d died\n",
						ft_delta(philo->start), philo->id + 1);
				id = philo->id;
				philo -= philo->id;
				jj = -1;
				while (++jj < philo->options.number_of_philosophers)
					if (jj != id)
						kill(philo[jj].pid, SIGINT);
			}
			sem_post(philo->options.screen);
			exit(0);
		}
	}
}

int	main(int argc, char *argv[])
{
	pid_t			pid;
	int				i;
	t_opts			options;
	pthread_t		*threads;
	t_philo			*philo;
	sem_t			*forks;

	if ((argc != 5 && argc != 6) || !is_valid_options(argv, &options))
		return (1);
	options.stop_simulation = FALSE;
	sem_unlink("forks");
	sem_unlink("scren");
	forks = sem_open("forks", O_CREAT, 0700, options.number_of_philosophers);
	options.screen = sem_open("scren", O_CREAT, 0700, 1);
	if (ft_alloc(options, &threads, &philo) == -1
			|| ft_init_philo(options, philo, forks) == -1)
		return (1);
	i = -1;
	while (++i < options.number_of_philosophers)
	{
		pid = fork();
		if (pid == 0)
		{
			pthread_create(&threads[i], NULL, ft_monitoring, &philo[i]);
			ft_philosopher(&philo[i]);
		}
		philo[i].pid = pid;
	}
	i = -1;
	while (++i < options.number_of_philosophers)
		waitpid(philo[i].pid, NULL, 0);
	return (0);
}
