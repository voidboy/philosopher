#include <stdio.h>
#include "philo.h"
#include "utils.h"

void	ft_free_philo(pthread_t *threads, t_philo *philo,
		pthread_mutex_t *forks, t_opts *options)
{
	int	i;

	i = -1;
	while (++i < philo->options->number_of_philosophers)
		pthread_mutex_destroy(&forks[i]);
	free(threads);
	free(philo);
	free(forks);
	free(options);
}

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

	pthread_mutex_lock(&p->options->screen);
	if (!p->options->stop_simulation)
		printf("%5ld %3d %s\n", ft_delta(p->start), p->id + 1, states[s]);
	pthread_mutex_unlock(&p->options->screen);
}

int	ft_fail_philo(int i, pthread_t *threads, t_philo *philo,
		pthread_mutex_t *forks)
{
	write(STDERR_FILENO, "pthread_mutex_init error\n", 25);
	while (--i >= 0)
		pthread_mutex_destroy(&forks[i]);
	free(threads);
	free(philo);
	free(forks);
	return (-1);
}

int	ft_init_philo(t_opts *options, pthread_t *threads, t_philo *philo,
		pthread_mutex_t *forks)
{
	struct timeval	start;
	int				i;

	if (gettimeofday(&start, NULL) == -1)
		return (-1);
	i = -1;
	while (++i < options->number_of_philosophers)
		if (pthread_mutex_init(&forks[i], NULL) == -1)
			return (ft_fail_philo(i, threads, philo, forks));
	i = -1;
	while (++i < options->number_of_philosophers)
	{
		if (i == options->number_of_philosophers - 1)
			philo[i] = (t_philo){i, options, &forks[i], &forks[0 + 0],
				0, start, start};
		else
			philo[i] = (t_philo){i, options, &forks[i], &forks[i + 1],
				0, start, start};
	}
	return (0);
}

int	ft_alloc(t_opts *options, pthread_t **threads, t_philo **philo,
		pthread_mutex_t **forks)
{
	*threads = malloc(sizeof(pthread_t) * options->number_of_philosophers);
	if (!*threads)
	{
		free(options);
		return (-1);
	}
	*philo = malloc(sizeof(t_philo) * options->number_of_philosophers);
	if (!*philo)
	{
		free(options);
		free(*threads);
		return (-1);
	}
	*forks = malloc(sizeof(pthread_mutex_t) * options->number_of_philosophers);
	if (!*forks)
	{
		free(options);
		free(*threads);
		free(*philo);
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
		if (ft_atoi(argv[5], &n) == FALSE || n <= 0)
			return (FALSE);
		options->must_eat = n;
	}
	else
		options->must_eat = -1;
	return (TRUE);
}

void	*ft_philosopher(void *philo)
{
	t_philo	*p;

	p = (t_philo *)philo;
	if (!(!p->id || !(p->id % 2)))
		usleep(p->options->time_to_eat);
	while (!p->options->stop_simulation)
	{
		pthread_mutex_lock(p->forkR);
		ft_show_state(p, FORK);
		pthread_mutex_lock(p->forkL);
		ft_show_state(p, FORK);
		ft_show_state(p, EAT);
		gettimeofday(&p->last_meal, NULL);
		usleep(p->options->time_to_eat);
		if (++p->numb_meal == p->options->must_eat)
			break ;
		pthread_mutex_unlock(p->forkR);
		pthread_mutex_unlock(p->forkL);
		ft_show_state(p, SLEEP);
		usleep(p->options->time_to_sleep);
		ft_show_state(p, THINK);
	}
	pthread_mutex_unlock(p->forkR);
	pthread_mutex_unlock(p->forkL);
	return (NULL);
}

void	ft_start_meal(pthread_t *threads, t_philo *philo)
{
	int	i;

	i = 0;
	while (i < philo->options->number_of_philosophers)
	{
		pthread_create(&threads[i], NULL, ft_philosopher, &philo[i]);
		pthread_detach(threads[i]);
		i += 2;
	}
	i = 1;
	while (i < philo->options->number_of_philosophers)
	{
		pthread_create(&threads[i], NULL, ft_philosopher, &philo[i]);
		pthread_detach(threads[i]);
		i += 2;
	}
}

void	ft_monitoring(t_philo *philo)
{
	int		i;

	while (1)
	{
		i = -1;
		while (++i < philo->options->number_of_philosophers)
		{
			if (ft_delta(philo[i].last_meal) >= philo[i].options->time_to_die)
			{
				pthread_mutex_lock(&philo[i].options->screen);
				philo[i].options->stop_simulation = TRUE;
				if (!(philo[i].numb_meal == philo[i].options->must_eat))
					printf("%5ld %3d died\n",
						ft_delta(philo->start), philo[i].id + 1);
				pthread_mutex_unlock(&philo[i].options->screen);
				return ;
			}
		}
	}
}

int	main(int argc, char *argv[])
{
	t_opts			*options;
	pthread_t		*threads;
	t_philo			*philo;
	pthread_mutex_t	*forks;

	options = malloc(sizeof(t_opts));
	if (!options || (argc != 5 && argc != 6)
		|| !is_valid_options(argv, options))
		return (1);
	options->stop_simulation = FALSE;
	if (pthread_mutex_init(&options->screen, NULL) == -1)
	{
		write(STDERR_FILENO, "pthread_mutex_init error\n", 25);
		free(options);
		return (1);
	}
	if (ft_alloc(options, &threads, &philo, &forks) == -1
		|| ft_init_philo(options, threads, philo, forks) == -1)
		return (1);
	ft_start_meal(threads, philo);
	ft_monitoring(philo);
	ft_free_philo(threads, philo, forks, options);
	return (0);
}
