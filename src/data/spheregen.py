import math

def generate_sphere(lat_sections, lon_sections):
    """
    Generate sphere vertices and edges for given latitude and longitude sections.
    lat_sections: number of horizontal divisions (minimum 2)
    lon_sections: number of vertical divisions (minimum 3)
    """
    vertices = []
    PI = math.pi

    # Top pole
    vertices.append((0.0, 1.0, 0.0))

    # Latitude rings (excluding poles)
    for lat in range(1, lat_sections):
        theta = PI * lat / lat_sections  # 0 to PI
        sin_theta = math.sin(theta)
        cos_theta = math.cos(theta)

        for lon in range(lon_sections):
            phi = 2.0 * PI * lon / lon_sections  # 0 to 2*PI
            sin_phi = math.sin(phi)
            cos_phi = math.cos(phi)

            x = sin_theta * cos_phi
            y = cos_theta
            z = sin_theta * sin_phi

            vertices.append((x, y, z))

    # Bottom pole
    vertices.append((0.0, -1.0, 0.0))

    # Generate edges
    edges = []

    # Top pole to first ring
    for i in range(1, lon_sections + 1):
        edges.append((0, i))

    # Meridian edges (vertical lines between rings)
    for ring in range(lat_sections - 2):
        ring_start = 1 + ring * lon_sections
        next_ring_start = 1 + (ring + 1) * lon_sections

        for i in range(lon_sections):
            edges.append((ring_start + i, next_ring_start + i))

    # Last ring to bottom pole
    last_ring_start = 1 + (lat_sections - 2) * lon_sections
    bottom_pole_idx = len(vertices) - 1

    for i in range(lon_sections):
        edges.append((last_ring_start + i, bottom_pole_idx))

    # Parallel edges (horizontal circles around each ring)
    for ring in range(lat_sections - 1):
        ring_start = 1 + ring * lon_sections

        for i in range(lon_sections):
            next_i = (i + 1) % lon_sections
            edges.append((ring_start + i, ring_start + next_i))

    return vertices, edges


def print_c_code(lat_sections, lon_sections):
    """
    Print sphere data in C code format ready to copy-paste.
    """
    vertices, edges = generate_sphere(lat_sections, lon_sections)

    print(f"// Sphere with {lat_sections} latitude sections and {lon_sections} longitude sections")
    print(f"// Total vertices: {len(vertices)}")
    print(f"// Total edges: {len(edges)}")
    print()

    # Print vertices
    print(f"struct Point3 sphereVertex_data[{len(vertices)}] = {{")
    for i, (x, y, z) in enumerate(vertices):
        print(f"    {{{x: 9.6f}f, {y:9.6f}f, {z:9.6f}f}},")
    print("};")
    print()

    # Print edges
    print(f"const int sphereEdges[{len(edges)}][2] = {{")

    # Print edges in rows of 6 for readability
    for i in range(0, len(edges), 6):
        edge_group = edges[i:i+6]
        line = "    " + ", ".join(f"{{{e[0]},{e[1]}}}" for e in edge_group)
        if i + 6 < len(edges):
            line += ","
        else:
            # Last line might need a comma if there are more edges
            if i + len(edge_group) < len(edges):
                line += ","
        print(line)

    print("};")


def main():
    print("=" * 70)
    print("SPHERE GENERATOR FOR C CODE")
    print("=" * 70)
    print()

    while True:
        try:
            lat = int(input("Enter number of latitude sections (e.g., 8): "))
            lon = int(input("Enter number of longitude sections (e.g., 12): "))

            if lat < 2:
                print("Error: Latitude sections must be at least 2")
                continue
            if lon < 3:
                print("Error:  Longitude sections must be at least 3")
                continue

            print()
            print("=" * 70)
            print_c_code(lat, lon)
            print("=" * 70)
            print()

            again = input("Generate another?  (y/n): ").strip().lower()
            if again != 'y':
                break

        except ValueError:
            print("Error: Please enter valid integers")
        except KeyboardInterrupt:
            print("\nExiting...")
            break


if __name__ == "__main__":
    main()
