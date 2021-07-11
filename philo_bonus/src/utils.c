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
