/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ft_substr.c                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: lsinke <lsinke@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/12/18 18:01:21 by lsinke        #+#    #+#                 */
/*   Updated: 2021/12/18 18:01:21 by lsinke        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*string;
	size_t	startpos;
	size_t	srclen;

	if (!s)
		return (NULL);
	srclen = ft_strlen(s);
	if (start > srclen || len == 0)
		return (ft_strdup(""));
	string = malloc((ft_min(srclen - start, len) + 1) * sizeof(char));
	if (string == NULL)
		return (NULL);
	if (start + len > srclen)
		startpos = srclen - start;
	else
		startpos = len;
	ft_memcpy(string, s + start, startpos);
	string[startpos] = '\0';
	return (string);
}
