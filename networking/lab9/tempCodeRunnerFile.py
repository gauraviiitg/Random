def ip_to_bin(ip):
    return ''.join(format(int(byte, 16), '08b') for byte in ip.split('.'))

def match_prefix(ip, prefix, mask_length):
    return ip[:mask_length] == prefix[:mask_length]

def get_next_hop(ip, routing_table):
    longest_match_length = -1
    next_hop = None

    for entry in routing_table:
        prefix, mask_length, hop = entry

        if match_prefix(ip, prefix, mask_length) and mask_length > longest_match_length:
            longest_match_length = mask_length
            next_hop = hop

    return next_hop

if __name__ == '__main__':
    routing_table_str = [
        ("C4.50.0.0/12", "A"),
        ("C4.5E.10.0/20", "B"),
        ("C4.60.0.0/12", "C"),
        ("C4.68.0.0/14", "D"),
        ("80.0.0.0/1", "E"),
        ("40.0.0.0/2", "F"),
        ("00.0.0.0/2", "G"),
    ]

    routing_table = []
    for net, hop in routing_table_str:
        ip, mask_length = net.split('/')
        routing_table.append((ip_to_bin(ip), int(mask_length), hop))

    test_ips = ["C4.5E.13.87", "C4.6B.31.2E","40.1.1.1"]
    for ip in test_ips:
        next_hop = get_next_hop(ip_to_bin(ip), routing_table)
        print(f"Next hop for {ip} is {next_hop}")
