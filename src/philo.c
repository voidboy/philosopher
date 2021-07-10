#include "philo.h"
#include "utils.h"

long	ft_delta(struct timeval start)
{
	struct timeval	delta;

	if (gettimeofday(&delta, NULL) == -1)
	{
		write(STDERR_FILENO, "gettimeofday error\n", 19);
		return (-1);
	}
	return ((delta.tv_sec * 1e3 + delta.tv_usec / 1e3)
			- (start.tv_sec * 1e3 + start.tv_usec / 1e3));
}

void ft_lock_screen(pthread_mutex_t *screen, t_states a, long h, int id)
{
	static const char *msg[] = {"has taken a fork", "is eating",
		"is sleeping", "is thinking", "died"};

	if (pthread_mutex_lock(screen) != 0)
		write(STDERR_FILENO, "pthread_mutex_lock error\n", 25);
	printf("%010ld %03d %s\n", h, id, msg[a]);
	if (pthread_mutex_unlock(screen) != 0)
		write(STDERR_FILENO, "pthread_mutex_unlock error\n", 27);
}

void	ft_nanosleep(long time_to_sleep, long last_meal, long time_to_die, long *starving)
{
	long time_slept;

	time_slept = -1;
	while (++time_slept < time_to_sleep)
	{
		if (last_meal + *starving >= last_meal + time_to_die)
		{
			printf("DIED !\n");
			exit(0);
		}
		usleep(1e3);
		(*starving) += 1;
	}
}

void ft_miam_miam(t_philo *p)
{
	if (pthread_mutex_lock(p->forkR) != 0)
		write(STDERR_FILENO, "pthread_mutex_lock error\n", 25);
	ft_lock_screen(&p->options.screen, FORK, ft_delta(p->start), p->philo + 1);
	if (pthread_mutex_lock(p->forkL) != 0)
		write(STDERR_FILENO, "pthread_mutex_lock error\n", 25);
	ft_lock_screen(&p->options.screen, FORK, ft_delta(p->start), p->philo + 1);
	p->last_meal = ft_delta((struct timeval){0, 0});
	p->starving = 0;
	ft_lock_screen(&p->options.screen, EAT, ft_delta(p->start), p->philo + 1);
	p->numb_meal++;
	ft_nanosleep(p->options.time_to_eat, p->last_meal, p->options.time_to_die, &p->starving);
	if (pthread_mutex_unlock(p->forkR) != 0)
		write(STDERR_FILENO, "pthread_mutex_unlock error\n", 27);
	if (pthread_mutex_unlock(p->forkL) != 0)
		write(STDERR_FILENO, "pthread_mutex_unlock error\n", 27);
}

void	*ft_philosopher(void *philo)
{
	t_philo	*p;

	p = (t_philo *)philo;
	p->last_meal = ft_delta((struct timeval){0, 0});
	if (!p->philo || !(p->philo % 2))
	{
		ft_lock_screen(&p->options.screen, THINK, ft_delta(p->start), p->philo + 1);
		ft_nanosleep(p->options.time_to_eat, p->last_meal, p->options.time_to_die, &p->starving);
	}
	while (p->options.must_eat == -1 || p->numb_meal < p->options.must_eat)
	{
		ft_miam_miam(p);
		ft_lock_screen(&p->options.screen, SLEEP, ft_delta(p->start), p->philo + 1);
		ft_nanosleep(p->options.time_to_sleep, p->last_meal, p->options.time_to_die, &p->starving);
		ft_lock_screen(&p->options.screen, THINK, ft_delta(p->start), p->philo + 1);
	}
	return (NULL);
}

int	ft_init_philo(t_opts options, pthread_mutex_t *forks, t_philo *philo)
{
	struct timeval	start;
	int				i;

	if (gettimeofday(&start, NULL) == -1)
		return (-1);
	i = -1;
	while (++i < options.number_of_philosophers)
	{
		if (pthread_mutex_init(&forks[i], NULL) == -1)
		{
			write(STDERR_FILENO, "pthread_mutex_init error\n", 25);
			while (--i >= 0)
				pthread_mutex_destroy(&forks[i]);
			return (-1);
		}
	}
	i = -1;
	while (++i < options.number_of_philosophers)
	{
		if (i == options.number_of_philosophers - 1)
			philo[i] = (t_philo){options, i, &forks[i], &forks[0], 0, 0, 0, start};
		else
			philo[i] = (t_philo){options, i, &forks[i], &forks[i + 1], 0, 0, 0, start};
	}
	return (0);
}

int	ft_alloc(int n, pthread_t **threads, t_philo **philo,
		pthread_mutex_t **forks)
{
	*threads = malloc(sizeof(pthread_t) * n);
	if (!*threads)
	{
		write(STDERR_FILENO, "malloc error\n", 13);
		return (-1);
	}
	*philo = malloc(sizeof(t_philo) * n);
	if (!*philo)
	{
		free(*threads);
		write(STDERR_FILENO, "malloc error\n", 13);
		return (-1);
	}
	*forks = malloc(sizeof(pthread_mutex_t) * n);
	if (!*forks)
	{
		free(*threads);
		free(*philo);
		write(STDERR_FILENO, "malloc error\n", 13);
		return (-1);
	}
	return (0);
}

t_bool is_valid_options(char *argv[], t_opts *options)
{
	int n;

	if (ft_atoi(argv[1], &n) == FALSE || n < 0)
		return (FALSE);
	options->number_of_philosophers = n;
	if (ft_atoi(argv[2], &n) == FALSE || n < 0)
		return (FALSE);
	options->time_to_die = n;
	if (ft_atoi(argv[3], &n) == FALSE || n < 0)
		return (FALSE);
	options->time_to_eat = n;
	if (ft_atoi(argv[4], &n) == FALSE || n < 0)
		return (FALSE);
	options->time_to_sleep = n;
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

void free_philo(int n, void *threads, void *philo, void *forks)
{
	free(threads);
	free(philo);
	while (--n >= 0)
		pthread_mutex_destroy(&(*((pthread_mutex_t *)forks + n)));
	free(forks);
}

void ft_atable(t_opts options, pthread_t *threads, t_philo *philo, pthread_mutex_t *forks)
{
	int				n;

	n = -1;
	while (++n < options.number_of_philosophers)
	{
		if (pthread_create(&threads[n], NULL, ft_philosopher, (void *)&philo[n]))
		{
			/* terminate already launched threads */
			free_philo(options.number_of_philosophers, threads, philo, forks);
			return ;
		}
	}
	n = -1;
	while (++n < options.number_of_philosophers)
	   pthread_join(threads[n], NULL);
   free_philo(options.number_of_philosophers, threads, philo, forks);
}

int	main(int argc, char *argv[])
{
	t_opts			options;
	pthread_t		*threads;
	t_philo			*philo;
	pthread_mutex_t *forks;

	if ((argc != 5 && argc != 6) || !is_valid_options(argv, &options))
		return (1);
	if (pthread_mutex_init(&options.screen, NULL) == -1)
	{
		write(STDERR_FILENO, "pthread_mutex_init error\n", 25);
		return (1);
	}
	if (ft_alloc(options.number_of_philosophers, &threads, &philo, &forks) == -1
			|| ft_init_philo(options, forks, philo) == -1)
		return (1);
	ft_atable(options, threads, philo, forks);
	return (0);
}
