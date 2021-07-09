#include "philo.h"

void ft_screen(pthread_mutex_t *screen, t_action a, long h, int id)
{
	static const char *txt[3] = {"eating", "sleeping", "thinking"};

	if (pthread_mutex_lock(screen) != 0)
		perror("OMG!\n");
	printf("%05ld %02d is %s\n", h, id, txt[a]);
	if (pthread_mutex_unlock(screen) != 0)
		perror("OMG!\n");
}

int	ft_isspace(char c)
{
	if ((c >= '\t' && c <= '\r') || c == ' ')
		return (1);
	else
		return (0);
}

int	ft_strcmp(const char *s1, const char *s2)
{
	unsigned int	i;

	i = 0;
	while ((s1[i] == s2[i]) && s1[i] != '\0')
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

t_bool	to_int(const char *str, int *n)
{
	int		neg;
	long	num;

	num = 0;
	neg = 0;
	while (ft_isspace(*str))
		str++;
	if (*str == '-' || *str == '+')
		if (*str++ == '-')
			neg = 1;
	*n = INT_MIN;
	if (neg && !ft_strcmp(str, "2147483648"))
		return (TRUE);
	while (*str && (*str >= '0' && *str <= '9'))
	{
		num = num * 10 + (*str++ - '0');
		if (num & 0xffffffff80000000)
			return (FALSE);
	}
	if (*str != '\0')
		return (FALSE);
	*n = num;
	if (neg)
		*n = -num;
	return (TRUE);
}

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

void	*aude_philosopher(void *philo)
{
	t_philo	*p;
	int		eaten;

	p = (t_philo *)philo;
	eaten = 0;
	while (p->options.must_eat == -1 || eaten < p->options.must_eat)
	{
		ft_screen(&p->options.screen, THINK, ft_delta(p->start), p->philo + 1);
		usleep(p->options.time_to_eat * 1e3);
		if (pthread_mutex_lock(p->forkL) != 0)
			perror("OMG!\n");
		if (pthread_mutex_lock(p->forkR) != 0)
			perror("OMG!\n");
		ft_screen(&p->options.screen, EAT, ft_delta(p->start), p->philo + 1);
		usleep(p->options.time_to_eat * 1e3);
		if (pthread_mutex_unlock(p->forkL) != 0)
			perror("OMG!\n");
		if (pthread_mutex_unlock(p->forkR) != 0)
			perror("OMG!\n");
		ft_screen(&p->options.screen, SLEEP, ft_delta(p->start), p->philo + 1);
		usleep(p->options.time_to_sleep * 1e3);
	}
	return (NULL);
}

void	*even_philosopher(void *philo)
{
	t_philo	*p;
	int		eaten;

	p = (t_philo *)philo;
	eaten = 0;
	while (p->options.must_eat == -1 || eaten < p->options.must_eat)
	{
		if (pthread_mutex_lock(p->forkL) != 0)
			perror("OMG!\n");
		if (pthread_mutex_lock(p->forkR) != 0)
			perror("OMG!\n");
		ft_screen(&p->options.screen, EAT, ft_delta(p->start), p->philo + 1);
		usleep(p->options.time_to_eat * 1e3);
		if (pthread_mutex_unlock(p->forkL) != 0)
			perror("OMG!\n");
		if (pthread_mutex_unlock(p->forkR) != 0)
			perror("OMG!\n");
		ft_screen(&p->options.screen, SLEEP, ft_delta(p->start), p->philo + 1);
		usleep(p->options.time_to_sleep * 1e3);
		ft_screen(&p->options.screen, THINK, ft_delta(p->start), p->philo + 1);
	}
	return (NULL);
}

void	*duck_philosopher(void *philo)
{
	t_philo	*p = (t_philo *)philo;
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
			philo[i] = (t_philo){options, i, &forks[i], &forks[0], start};
		else
			philo[i] = (t_philo){options, i, &forks[i], &forks[i + 1], start};
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

	if (to_int(argv[1], &n) == FALSE || n < 0)
		return (FALSE);
	options->number_of_philosophers = n;
	if (to_int(argv[2], &n) == FALSE || n < 0)
		return (FALSE);
	options->time_to_die = n;
	if (to_int(argv[3], &n) == FALSE || n < 0)
		return (FALSE);
	options->time_to_eat = n;
	if (to_int(argv[4], &n) == FALSE || n < 0)
		return (FALSE);
	options->time_to_sleep = n;
	if (argv[5])
	{
		if (to_int(argv[5], &n) == FALSE || n < 0)
			return (FALSE);
		options->must_eat = n;
	}
	else 
		options->must_eat = -1;
	if (pthread_mutex_init(&options->screen, NULL) == -1)
	{
		write(STDERR_FILENO, "pthread_mutex_init error\n", 25);
		return (FALSE);
	}
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
	void			*(*f)(void *);

	n = -1;
	while (++n < options.number_of_philosophers)
	{
		if (!n || !(n % 2))
			f = aude_philosopher;
		else 
			f = even_philosopher;
		if (pthread_create(&threads[n], NULL, f, (void *)&philo[n]))
		{
			/* terminate already launched threads */
			free_philo(options.number_of_philosophers, threads, philo, forks);
			return ;
		}
		pthread_detach(threads[n]);
	}
	/*
	   pthread_join(threads[0], NULL);
	   pthread_join(threads[1], NULL);
	   pthread_join(threads[2], NULL);
	   free_philo(options.number_of_philosophers, threads, philo, forks);
	   */
}

int	main(int argc, char *argv[])
{
	t_opts			options;
	pthread_t		*threads;
	t_philo			*philo;
	pthread_mutex_t *forks;

	if ((argc != 5 && argc != 6) || !is_valid_options(argv, &options))
		return (1);
	if (ft_alloc(options.number_of_philosophers, &threads, &philo, &forks) == -1
			|| ft_init_philo(options, forks, philo) == -1)
		return (1);
	ft_atable(options, threads, philo, forks);
	while (1)
		;
	return (0);
}
