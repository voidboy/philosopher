#ifndef UTILS_H
# define UTILS_H
int		ft_isspace(char c);
int		ft_strcmp(const char *s1, const char *s2);
t_bool	ft_atoi(const char *str, int *n);
t_bool	is_valid_options(char *argv[], t_opts *options);
long	ft_delta(struct timeval ref);
#endif 
