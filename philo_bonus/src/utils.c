#include "philo.h"

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

long	ft_delta(struct timeval ref)
{
	struct timeval	delta;

	gettimeofday(&delta, NULL);
	return ((delta.tv_sec * 1e3 + delta.tv_usec * 1e-3)
		- (ref.tv_sec * 1e3 + ref.tv_usec * 1e-3));
}

t_bool	ft_atoi(const char *str, int *n)
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

t_bool	is_valid_options(char *argv[], t_opts *options)
{
	int	n;

	if (ft_atoi(argv[1], &n) == FALSE || n <= 0)
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
