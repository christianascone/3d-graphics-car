Riccardo Candido - 2° Homework:

Homework 1 - persp_mesh.c:
- Si proceda all'interno della directory di progetto
- Si esegua il comando make
- Si esegua il comando ./persp_mesh
- Qui sono state implementate come richiesto dalla consegna 3 punti sui 4 proposti:
	1 -> 	E' possibile ruotare l'oggetto cliccandoci sopra. L'oggetto si sposterà:
	    	- Su e giù se si muove il mouse sull'asse delle y
		- A destra e a sinistra se si muove il mouse sull'asse delle x
	2 -> 	E' possibile vedere il render di 4 cubi. I quali sono stati plotatti/disegnati
		come nell'esempio mostrato a lezione
	3 -> 	E' stato implmentato il meccanismo di view up, il quale permette al variare della
		prospettiva di modificare la visualizzazione dell'oggetto come se lo spettatore 
		inclinasse la visuale. Questo è stato ottenuto calcolando la matrice B^-1 indicata
		a lezione
	4 -> 	Per quanto concerne lo step numero 4 richiesto, si hanno dei problemi nella 
		visualizzazione della seconda windows la quale lancia costantemente errori di 
		segmentation fault i quali non si riescono a debuggare. 
