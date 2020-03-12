# Morfogenesi

Il kernel usato per integrare Grey-Scott:

```GLSL
uniform sampler2D t_seed;

uniform float u_Dt; //! = 0.8
uniform float u_Da; //! = 1.0
uniform float u_Db; //! = 0.5
uniform float u_f;
uniform float u_k;

uniform ivec2 u_screen; //! DIMENSIONE DELLA FINESTRA
uniform ivec2 u_canvas; //! DIMENSIONE DEL BUFFER
uniform ivec2 u_pencil; //! POSIZIONE DEL CURSORE IN COORDINATE DELLA FINESTRA (0, 0) ANGOLO ALTO A SINISTRA

in vec2 f_uv; //! COORDINATE DEL PIXEL NORMALIZZATE [0-1][0-1]

float sqrLength(vec2 v)
{
	return dot(v, v);
}
vec2 getBrush()
{
	return vec2(u_pencil.x, u_screen.y - u_pencil.y) - f_uv * u_screen; //! IN OPENGL (PURTROPPO) L'ORIGINE E' IN BASSO A SINISTRA
}

void main()
{
	vec2 step = 1.f / u_canvas;

	vec4 t = texture2D(t_seed, f_uv + vec2(0, +step.y)); //! CAMPIONE PIXEL ADIACENTE
	vec4 b = texture2D(t_seed, f_uv + vec2(0, -step.y)); //! CAMPIONE PIXEL ADIACENTE
	vec4 l = texture2D(t_seed, f_uv + vec2(-step.x, 0)); //! CAMPIONE PIXEL ADIACENTE
	vec4 r = texture2D(t_seed, f_uv + vec2(+step.x, 0)); //! CAMPIONE PIXEL ADIACENTE
	vec4 c = texture2D(t_seed, f_uv);

	vec4 lap = ((t + b + l + r) - 4 * c) / 5; //! LAPLACIANO

	float A = c.x + ((u_Da * lap.x - c.x * c.y * c.y) + u_f * (1.0 - c.x)) * u_Dt;
	float B = c.y + ((u_Db * lap.y + c.x * c.y * c.y) - c.y * (u_k + u_f)) * u_Dt;

	if (u_pencil.x > 0.f)
	{
		if (sqrLength(getBrush()) < 10.f) B = .9;
	}

	gl_FragColor.r = A;
	gl_FragColor.g = B;
	gl_FragColor.b = 0; //! VA SCRITTO PER FORZA 
	gl_FragColor.a = 1; //! VA SCRITTO PER FORZA
}
```
